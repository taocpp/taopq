// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/transaction.hpp>

#include <chrono>
#include <cstdio>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>
#include <tao/pq/pipeline.hpp>

namespace tao::pq
{
   namespace internal
   {
      class top_level_subtransaction final
         : public subtransaction_base
      {
      public:
         explicit top_level_subtransaction( const std::shared_ptr< pq::connection >& connection )
            : subtransaction_base( connection )
         {
            execute( "START TRANSACTION" );
         }

         ~top_level_subtransaction() override
         {
            if( m_connection && m_connection->attempt_rollback() ) {
               try {
                  rollback();
               }
               // LCOV_EXCL_START
               catch( const std::exception& ) {  // NOLINT(bugprone-empty-catch)
                  // TAO_LOG( WARNING, "unable to rollback transaction, swallowing exception: " + std::string( e.what() ) );
               }
               catch( ... ) {  // NOLINT(bugprone-empty-catch)
                  // TAO_LOG( WARNING, "unable to rollback transaction, swallowing unknown exception" );
               }
               // LCOV_EXCL_STOP
            }
         }

         top_level_subtransaction( const top_level_subtransaction& ) = delete;
         top_level_subtransaction( top_level_subtransaction&& ) = delete;
         void operator=( const top_level_subtransaction& ) = delete;
         void operator=( top_level_subtransaction&& ) = delete;

      private:
         void v_commit() override
         {
            execute( "COMMIT TRANSACTION" );
         }

         void v_rollback() override
         {
            execute( "ROLLBACK TRANSACTION" );
         }
      };

      class nested_subtransaction final
         : public subtransaction_base
      {
      public:
         explicit nested_subtransaction( const std::shared_ptr< pq::connection >& connection )
            : subtransaction_base( connection )
         {
            char buffer[ 64 ];
            std::snprintf( buffer, 64, "SAVEPOINT \"TAOPQ_%p\"", static_cast< void* >( this ) );
            execute( buffer );
         }

         ~nested_subtransaction() override
         {
            if( m_connection && m_connection->attempt_rollback() ) {
               try {
                  rollback();
               }
               // LCOV_EXCL_START
               catch( ... ) {  // NOLINT(bugprone-empty-catch)
                  // TODO: How to handle this case properly?
               }
               // LCOV_EXCL_STOP
            }
         }

         nested_subtransaction( const nested_subtransaction& ) = delete;
         nested_subtransaction( nested_subtransaction&& ) = delete;
         void operator=( const nested_subtransaction& ) = delete;
         void operator=( nested_subtransaction&& ) = delete;

      private:
         void v_commit() override
         {
            char buffer[ 64 ];
            std::snprintf( buffer, 64, "RELEASE SAVEPOINT \"TAOPQ_%p\"", static_cast< void* >( this ) );
            execute( buffer );
         }

         void v_rollback() override
         {
            char buffer[ 64 ];
            std::snprintf( buffer, 64, "ROLLBACK TO \"TAOPQ_%p\"", static_cast< void* >( this ) );
            execute( buffer );
         }
      };

   }  // namespace internal

   auto transaction::subtransaction() -> std::shared_ptr< transaction >
   {
      check_current_transaction();
      if( v_is_direct() ) {
         return std::make_shared< internal::top_level_subtransaction >( m_connection );
      }
      return std::make_shared< internal::nested_subtransaction >( m_connection );
   }

   auto transaction::pipeline() -> std::shared_ptr< pq::pipeline >
   {
      check_current_transaction();
      return std::make_shared< pq::pipeline >( m_connection );
   }

   void transaction::commit()
   {
      check_current_transaction();
      try {
         v_commit();
      }
      // LCOV_EXCL_START
      catch( ... ) {
         v_reset();
         throw;
      }
      // LCOV_EXCL_STOP
      v_reset();
   }

   void transaction::rollback()
   {
      check_current_transaction();
      try {
         v_rollback();
      }
      // LCOV_EXCL_START
      catch( ... ) {
         v_reset();
         throw;
      }
      // LCOV_EXCL_STOP
      v_reset();
   }

}  // namespace tao::pq
