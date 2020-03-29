// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/transaction.hpp>

#include <stdexcept>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/printf.hpp>

namespace tao::pq
{
   namespace
   {
      class transaction_base
         : public transaction
      {
      private:
         const std::shared_ptr< transaction > m_previous;

      protected:
         explicit transaction_base( const std::shared_ptr< pq::connection >& connection )
            : transaction( connection ),
              m_previous( current_transaction()->shared_from_this() )
         {
            current_transaction() = this;
         }

         ~transaction_base() override
         {
            if( m_connection ) {
               current_transaction() = m_previous.get();  // LCOV_EXCL_LINE
            }
         }

         [[nodiscard]] auto v_is_direct() const noexcept -> bool override
         {
            return false;
         }

         void v_reset() noexcept override
         {
            current_transaction() = m_previous.get();
            m_connection.reset();
         }

      public:
         transaction_base( const transaction_base& ) = delete;
         transaction_base( transaction_base&& ) = delete;
         void operator=( const transaction_base& ) = delete;
         void operator=( transaction_base&& ) = delete;
      };

      class top_level_transaction final
         : public transaction_base
      {
      public:
         explicit top_level_transaction( const std::shared_ptr< pq::connection >& connection )
            : transaction_base( connection )
         {
            execute( "START TRANSACTION" );
         }

         ~top_level_transaction() override
         {
            if( m_connection && m_connection->is_open() ) {
               try {
                  rollback();
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

         top_level_transaction( const top_level_transaction& ) = delete;
         top_level_transaction( top_level_transaction&& ) = delete;
         void operator=( const top_level_transaction& ) = delete;
         void operator=( top_level_transaction&& ) = delete;

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

      class nested_transaction final
         : public transaction_base
      {
      public:
         explicit nested_transaction( const std::shared_ptr< connection >& connection )
            : transaction_base( connection )
         {
            execute( internal::printf( "SAVEPOINT \"TAOPQ_%p\"", static_cast< void* >( this ) ) );
         }

         ~nested_transaction() override
         {
            if( m_connection && m_connection->is_open() ) {
               try {
                  rollback();
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

         nested_transaction( const nested_transaction& ) = delete;
         nested_transaction( nested_transaction&& ) = delete;
         void operator=( const nested_transaction& ) = delete;
         void operator=( nested_transaction&& ) = delete;

      private:
         void v_commit() override
         {
            execute( internal::printf( "RELEASE SAVEPOINT \"TAOPQ_%p\"", static_cast< void* >( this ) ) );
         }

         void v_rollback() override
         {
            execute( internal::printf( "ROLLBACK TO \"TAOPQ_%p\"", static_cast< void* >( this ) ) );
         }
      };

   }  // namespace

   transaction::transaction( const std::shared_ptr< pq::connection >& connection )  // NOLINT(modernize-pass-by-value)
      : m_connection( connection )
   {
   }

   transaction::~transaction() = default;

   auto transaction::current_transaction() const noexcept -> transaction*&
   {
      return m_connection->m_current_transaction;
   }

   void transaction::check_current_transaction() const
   {
      if( !m_connection || this != current_transaction() ) {
         throw std::logic_error( "transaction order error" );
      }
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

   auto transaction::subtransaction() -> std::shared_ptr< transaction >
   {
      check_current_transaction();
      if( v_is_direct() ) {
         return std::make_shared< top_level_transaction >( m_connection );
      }
      return std::make_shared< nested_transaction >( m_connection );
   }

}  // namespace tao::pq
