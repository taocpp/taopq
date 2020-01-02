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

      [[nodiscard]] std::unique_ptr< pq::connection > v_create() const override;
      [[nodiscard]] bool v_is_valid( pq::connection& c ) const noexcept override;

   public:
      [[nodiscard]] static std::shared_ptr< connection_pool > create( const std::string& connection_info );

   private:
      // pass-key idiom
      class private_key
      {
         private_key() = default;
         friend std::shared_ptr< connection_pool > connection_pool::create( const std::string& connect_info );
      };

   public:
      connection_pool( const private_key&, const std::string& connection_info );

      [[nodiscard]] std::shared_ptr< pq::connection > connection()
      {
         return this->get();
      }

      template< typename... Ts >
      result execute( Ts&&... ts )
      {
         return this->connection()->direct()->execute( std::forward< Ts >( ts )... );
      }
   };

}  // namespace tao::pq

#endif
