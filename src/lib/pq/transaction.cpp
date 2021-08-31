// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <stdexcept>

#include <tao/pq/connection.hpp>
#include <tao/pq/oid.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   namespace internal
   {
      class top_level_subtransaction final
         : public subtransaction_base
      {
      public:
         explicit top_level_subtransaction( const std::shared_ptr< connection >& connection )
            : subtransaction_base( connection )
         {
            this->execute( "START TRANSACTION" );
         }

         ~top_level_subtransaction() override
         {
            if( this->m_connection && this->m_connection->is_open() ) {
               try {
                  this->rollback();
               }
               // LCOV_EXCL_START
               catch( const std::exception& ) {
                  // TAO_LOG( WARNING, "unable to rollback transaction, swallowing exception: " + std::string( e.what() ) );
               }
               catch( ... ) {
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
            this->execute( "COMMIT TRANSACTION" );
         }

         void v_rollback() override
         {
            this->execute( "ROLLBACK TRANSACTION" );
         }
      };

      class nested_subtransaction final
         : public subtransaction_base
      {
      public:
         explicit nested_subtransaction( const std::shared_ptr< connection >& connection )
            : subtransaction_base( connection )
         {
            char buffer[ 64 ];
            std::snprintf( buffer, 64, "SAVEPOINT \"TAOPQ_%p\"", static_cast< void* >( this ) );
            this->execute( buffer );
         }

         ~nested_subtransaction() override
         {
            if( this->m_connection && this->m_connection->is_open() ) {
               try {
                  this->rollback();
               }
               // LCOV_EXCL_START
               catch( const std::exception& ) {
                  // TODO: Add more information about exception when available
                  // TAO_LOG( WARNING, "unable to rollback transaction, swallowing exception: " + std::string( e.what() ) );
               }
               catch( ... ) {
                  // TAO_LOG( WARNING, "unable to rollback transaction, swallowing unknown exception" );
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
            this->execute( buffer );
         }

         void v_rollback() override
         {
            char buffer[ 64 ];
            std::snprintf( buffer, 64, "ROLLBACK TO \"TAOPQ_%p\"", static_cast< void* >( this ) );
            this->execute( buffer );
         }
      };

   }  // namespace internal

   transaction::transaction( const std::shared_ptr< connection >& connection )  // NOLINT(modernize-pass-by-value)
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

   auto transaction::error_message() const -> std::string
   {
      check_current_transaction();
      return m_connection->error_message();
   }

   auto transaction::execute_params( const char* statement,
                                     const int n_params,
                                     const Oid types[],
                                     const char* const values[],
                                     const int lengths[],
                                     const int formats[] ) -> result
   {
      check_current_transaction();
      return m_connection->execute_params( statement, n_params, types, values, lengths, formats );
   }

   auto transaction::underlying_raw_ptr() const noexcept -> PGconn*
   {
      return m_connection->underlying_raw_ptr();
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
