// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
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
#include <tao/pq/oid.hpp>
#include <tao/pq/result.hpp>

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

   transaction::transaction( const std::shared_ptr< pq::connection >& connection )  // NOLINT(modernize-pass-by-value)
      : m_connection( connection )
   {}

   auto transaction::current_transaction() const noexcept -> transaction*&
   {
      return m_connection->m_current_transaction;
   }

   void transaction::check_current_transaction() const
   {
      if( !m_connection || this != current_transaction() ) {
         throw std::logic_error( "invalid transaction order" );
      }
   }

   void transaction::send_params( const char* statement,
                                  const int n_params,
                                  const Oid types[],
                                  const char* const values[],
                                  const int lengths[],
                                  const int formats[] )
   {
      check_current_transaction();
      m_connection->send_params( statement, n_params, types, values, lengths, formats );
   }

   auto transaction::get_result( const std::chrono::steady_clock::time_point start ) -> result
   {
      check_current_transaction();
      const auto end = m_connection->timeout_end( start );

      auto result = m_connection->get_result( end );
      if( result ) {
         switch( PQresultStatus( result.get() ) ) {
            case PGRES_COPY_IN:
               m_connection->put_copy_end( "unexpected COPY FROM statement" );
               break;

            case PGRES_COPY_OUT:
               m_connection->cancel();
               m_connection->clear_copy_data( end );
               m_connection->clear_results( end );
               throw std::runtime_error( "unexpected COPY TO statement" );

            default:;
         }
         while( auto next = m_connection->get_result( end ) ) {
            result = std::move( next );
         }
      }

      return pq::result( result.release() );
   }

   auto transaction::subtransaction() -> std::shared_ptr< transaction >
   {
      check_current_transaction();
      if( v_is_direct() ) {
         return std::make_shared< internal::top_level_subtransaction >( m_connection );
      }
      return std::make_shared< internal::nested_subtransaction >( m_connection );
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
