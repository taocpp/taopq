// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_CONNECTION_HPP
#define TAO_PQ_CONNECTION_HPP

#include <memory>
#include <string>
#include <utility>

#include <tao/pq/internal/connection.hpp>
#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/isolation_level.hpp>
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   namespace internal
   {
      template< template< typename... > class Traits >
      class transaction_base
         : public pq::transaction< Traits >
      {
      protected:
         explicit transaction_base( const std::shared_ptr< connection >& connection )
            : pq::transaction< Traits >( connection )
         {
            if( this->current_transaction() != nullptr ) {
               throw std::logic_error( "transaction order error" );
            }
            this->current_transaction() = this;
         }

         ~transaction_base() override
         {
            if( this->m_connection ) {
               this->current_transaction() = nullptr;
            }
         }

         void v_reset() noexcept override
         {
            this->current_transaction() = nullptr;
            this->m_connection.reset();
         }

      public:
         transaction_base( const transaction_base& ) = delete;
         transaction_base( transaction_base&& ) = delete;
         void operator=( const transaction_base& ) = delete;
         void operator=( transaction_base&& ) = delete;
      };

      template< template< typename... > class Traits >
      class autocommit_transaction final
         : public transaction_base< Traits >
      {
      public:
         explicit autocommit_transaction( const std::shared_ptr< connection >& connection )
            : transaction_base< Traits >( connection )
         {}

      private:
         [[nodiscard]] auto v_is_direct() const noexcept -> bool override
         {
            return true;
         }

         void v_commit() override
         {}

         void v_rollback() override
         {}
      };

      template< template< typename... > class Traits >
      class top_level_transaction final
         : public transaction_base< Traits >
      {
      private:
         [[nodiscard]] static auto isolation_level_to_statement( const isolation_level il ) -> const char*
         {
            switch( il ) {
               case isolation_level::default_isolation_level:
                  return "START TRANSACTION";
               case isolation_level::serializable:
                  return "START TRANSACTION ISOLATION LEVEL SERIALIZABLE";
               case isolation_level::repeatable_read:
                  return "START TRANSACTION ISOLATION LEVEL REPEATABLE READ";
               case isolation_level::read_committed:
                  return "START TRANSACTION ISOLATION LEVEL READ COMMITTED";
               case isolation_level::read_uncommitted:
                  return "START TRANSACTION ISOLATION LEVEL READ UNCOMMITTED";
            }
            TAO_PQ_UNREACHABLE;
         }

      public:
         explicit top_level_transaction( const isolation_level il, const std::shared_ptr< connection >& connection )
            : transaction_base< Traits >( connection )
         {
            this->execute( isolation_level_to_statement( il ) );
         }

         ~top_level_transaction() override
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

         top_level_transaction( const top_level_transaction& ) = delete;
         top_level_transaction( top_level_transaction&& ) = delete;
         void operator=( const top_level_transaction& ) = delete;
         void operator=( top_level_transaction&& ) = delete;

      private:
         [[nodiscard]] auto v_is_direct() const noexcept -> bool override
         {
            return false;
         }

         void v_commit() override
         {
            this->execute( "COMMIT TRANSACTION" );
         }

         void v_rollback() override
         {
            this->execute( "ROLLBACK TRANSACTION" );
         }
      };

   }  // namespace internal

   template< template< typename... > class DefaultTraits >
   class basic_connection final
      : public internal::connection
   {
   public:
      explicit basic_connection( const std::string& connection_info )
         : internal::connection( connection_info )
      {}

      basic_connection( const basic_connection& ) = delete;
      basic_connection( basic_connection&& ) = delete;
      void operator=( const basic_connection& ) = delete;
      void operator=( basic_connection&& ) = delete;

      ~basic_connection() = default;

      [[nodiscard]] static auto create( const std::string& connection_info )
      {
         return std::make_shared< basic_connection >( connection_info );
      }

      template< template< typename... > class Traits = DefaultTraits >
      [[nodiscard]] auto direct() -> std::shared_ptr< pq::transaction< Traits > >
      {
         return std::make_shared< internal::autocommit_transaction< Traits > >( shared_from_this() );
      }

      template< template< typename... > class Traits = DefaultTraits >
      [[nodiscard]] auto transaction( const isolation_level il = isolation_level::default_isolation_level ) -> std::shared_ptr< pq::transaction< Traits > >
      {
         return std::make_shared< internal::top_level_transaction< Traits > >( il, shared_from_this() );
      }

      template< template< typename... > class Traits = DefaultTraits, typename... Ts >
      auto execute( Ts&&... ts )
      {
         return this->template direct< Traits >()->execute( std::forward< Ts >( ts )... );
      }
   };

   using connection = basic_connection< parameter_text_traits >;
   using bconnection = basic_connection< parameter_binary_traits >;

}  // namespace tao::pq

#endif
