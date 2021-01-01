// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_CONNECTION_POOL_HPP
#define TAO_PQ_CONNECTION_POOL_HPP

#include <memory>
#include <string>
#include <utility>

#include <tao/pq/internal/pool.hpp>

#include <tao/pq/connection.hpp>
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   template< template< typename... > class DefaultTraits >
   class basic_connection_pool
      : public internal::pool< basic_connection< DefaultTraits > >
   {
   private:
      const std::string m_connection_info;

      [[nodiscard]] auto v_create() const -> std::unique_ptr< basic_connection< DefaultTraits > > override
      {
         return std::make_unique< basic_connection< DefaultTraits > >( m_connection_info );
      }

      [[nodiscard]] auto v_is_valid( basic_connection< DefaultTraits >& c ) const noexcept -> bool override
      {
         return c.is_open();
      }

      // pass-key idiom
      class private_key
      {
         private_key() = default;
         friend auto basic_connection_pool::create( const std::string& connection_info ) -> std::shared_ptr< basic_connection_pool >;
      };

   public:
      basic_connection_pool( const private_key& /*unused*/, const std::string& connection_info ) noexcept  // NOLINT(modernize-pass-by-value)
         : m_connection_info( connection_info )
      {}

      [[nodiscard]] static auto create( const std::string& connection_info ) -> std::shared_ptr< basic_connection_pool >
      {
         return std::make_shared< basic_connection_pool >( private_key(), connection_info );
      }

      [[nodiscard]] auto connection()
      {
         return this->get();
      }

      template< template< typename... > class Traits = DefaultTraits, typename... Ts >
      auto execute( Ts&&... ts )
      {
         return this->connection()->direct()->template execute< Traits >( std::forward< Ts >( ts )... );
      }
   };

   using connection_pool = basic_connection_pool< parameter_text_traits >;
   using bconnection_pool = basic_connection_pool< parameter_binary_traits >;

}  // namespace tao::pq

#endif
