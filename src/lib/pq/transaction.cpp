// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
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

         ~transaction_base()
         {
            if( m_connection ) {
               current_transaction() = m_previous.get();  // LCOV_EXCL_LINE
            }
         }

         [[nodiscard]] bool v_is_direct() const
         {
            return false;
         }

         void v_reset() noexcept
         {
            current_transaction() = m_previous.get();
            m_connection.reset();
         }
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

         ~top_level_transaction()
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

      private:
         void v_commit()
         {
            execute( "COMMIT TRANSACTION" );
         }

         void v_rollback()
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

         ~nested_transaction()
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

      private:
         void v_commit()
         {
            execute( internal::printf( "RELEASE SAVEPOINT \"TAOPQ_%p\"", static_cast< void* >( this ) ) );
         }

         void v_rollback()
         {
            execute( internal::printf( "ROLLBACK TO \"TAOPQ_%p\"", static_cast< void* >( this ) ) );
         }
      };
   }  // namespace

   transaction::transaction( const std::shared_ptr< pq::connection >& connection )
      : m_connection( connection )
   {
   }

   transaction::~transaction()
   {
   }

   transaction*& transaction::current_transaction() const noexcept
   {
      return m_connection->m_current_transaction;
   }

   void transaction::check_current_transaction() const
   {
      if( !m_connection || this != current_transaction() ) {
         throw std::logic_error( "transaction order error" );
      }
   }

   result transaction::execute_params( const char* statement,
                                       const int n_params,
                                       const char* const param_values[],
                                       const int param_lengths[],
                                       const int param_formats[] )
   {
      check_current_transaction();
      return m_connection->execute_params( statement, n_params, param_values, param_lengths, param_formats );
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

   std::shared_ptr< transaction > transaction::subtransaction()
   {
      check_current_transaction();
      if( v_is_direct() ) {
         return std::make_shared< top_level_transaction >( m_connection );
      }
      else {
         return std::make_shared< nested_transaction >( m_connection );
      }
   }

}  // namespace tao::pq
