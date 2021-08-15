// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TRANSACTION_HPP
#define TAO_PQ_TRANSACTION_HPP

#include <cstdio>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/to_traits.hpp>
#include <tao/pq/internal/transaction.hpp>

namespace tao::pq
{
   namespace internal
   {
      class connection;

   }  // namespace internal

   template< template< typename... > class DefaultTraits >
   class transaction
      : public internal::transaction
   {
   public:
      explicit transaction( const std::shared_ptr< internal::connection >& connection )
         : internal::transaction( connection )
      {}

      ~transaction() override = default;

      transaction( const transaction& ) = delete;
      transaction( transaction&& ) = delete;
      void operator=( const transaction& ) = delete;
      void operator=( transaction&& ) = delete;

      template< template< typename... > class Traits = DefaultTraits >
      [[nodiscard]] auto subtransaction() -> std::shared_ptr< pq::transaction< Traits > >;

      template< template< typename... > class Traits = DefaultTraits, typename... As >
      auto execute( const char* statement, As&&... as )
      {
         if constexpr( sizeof...( As ) == 0 ) {
            return execute_params( statement, 0, nullptr, nullptr, nullptr, nullptr );
         }
         else {
            return execute_traits( statement, internal::to_traits< Traits >( underlying_raw_ptr(), std::forward< As >( as ) )... );
         }
      }

      template< template< typename... > class Traits = DefaultTraits, typename... As >
      auto execute( const std::string& statement, As&&... as )
      {
         return execute< Traits >( statement.c_str(), std::forward< As >( as )... );
      }
   };

   namespace internal
   {
      template< template< typename... > class Traits >
      class subtransaction_base
         : public pq::transaction< Traits >
      {
      private:
         const std::shared_ptr< internal::transaction > m_previous;

      protected:
         explicit subtransaction_base( const std::shared_ptr< connection >& connection )
            : pq::transaction< Traits >( connection ),
              m_previous( this->current_transaction()->shared_from_this() )
         {
            this->current_transaction() = this;
         }

         ~subtransaction_base() override
         {
            if( this->m_connection ) {
               this->current_transaction() = m_previous.get();  // LCOV_EXCL_LINE
            }
         }

         [[nodiscard]] auto v_is_direct() const noexcept -> bool override
         {
            return false;
         }

         void v_reset() noexcept override
         {
            this->current_transaction() = m_previous.get();
            this->m_connection.reset();
         }

      public:
         subtransaction_base( const subtransaction_base& ) = delete;
         subtransaction_base( subtransaction_base&& ) = delete;
         void operator=( const subtransaction_base& ) = delete;
         void operator=( subtransaction_base&& ) = delete;
      };

      template< template< typename... > class Traits >
      class top_level_subtransaction final
         : public subtransaction_base< Traits >
      {
      public:
         explicit top_level_subtransaction( const std::shared_ptr< connection >& connection )
            : subtransaction_base< Traits >( connection )
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

      template< template< typename... > class Traits >
      class nested_subtransaction final
         : public subtransaction_base< Traits >
      {
      public:
         explicit nested_subtransaction( const std::shared_ptr< connection >& connection )
            : subtransaction_base< Traits >( connection )
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

      template< typename >
      class unused_traits;

      class transaction_guard final
         : public subtransaction_base< unused_traits >
      {
      public:
         explicit transaction_guard( const std::shared_ptr< connection >& connection )
            : subtransaction_base< unused_traits >( connection )
         {}

      private:
         // LCOV_EXCL_START
         void v_commit() override {}
         void v_rollback() override {}
         // LCOV_EXCL_STOP
      };

   }  // namespace internal

   template< template< typename... > class DefaultTraits >
   template< template< typename... > class Traits >
   auto transaction< DefaultTraits >::subtransaction() -> std::shared_ptr< pq::transaction< Traits > >
   {
      check_current_transaction();
      if( v_is_direct() ) {
         return std::make_shared< internal::top_level_subtransaction< Traits > >( m_connection );
      }
      return std::make_shared< internal::nested_subtransaction< Traits > >( m_connection );
   }

}  // namespace tao::pq

#endif
