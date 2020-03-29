// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_CONNECTION_POOL_HPP
#define TAO_PQ_CONNECTION_POOL_HPP

#include <memory>
#include <string>
#include <utility>

#include <tao/pq/internal/pool.hpp>

#include <tao/pq/connection.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   class connection_pool
      : public internal::pool< pq::connection >
   {
   private:
      const std::string m_connection_info;

      [[nodiscard]] auto v_create() const -> std::unique_ptr< pq::connection > override;

      [[nodiscard]] auto v_is_valid( pq::connection& c ) const noexcept -> bool override;

   public:
      [[nodiscard]] static auto create( const std::string& connection_info ) -> std::shared_ptr< connection_pool >;

   private:
      // pass-key idiom
      class private_key
      {
         private_key() = default;
         friend auto connection_pool::create( const std::string& connection_info ) -> std::shared_ptr< connection_pool >;
      };

   public:
      connection_pool( const private_key& /*unused*/, const std::string& connection_info ) noexcept  // NOLINT(modernize-pass-by-value)
         : m_connection_info( connection_info )
      {}

      [[nodiscard]] auto connection()
      {
         return this->get();
      }

      template< template< typename... > class Traits = parameter_text_traits, typename... Ts >
      auto execute( Ts&&... ts )
      {
         return this->connection()->direct()->execute< Traits >( std::forward< Ts >( ts )... );
      }
   };

}  // namespace tao::pq

#endif
