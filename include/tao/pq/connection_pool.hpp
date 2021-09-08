// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_CONNECTION_POOL_HPP
#define TAO_PQ_CONNECTION_POOL_HPP

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/pool.hpp>
#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   class connection_pool final
      : public internal::pool< connection >
   {
   private:
      const std::string m_connection_info;

      [[nodiscard]] auto v_create() const -> std::unique_ptr< pq::connection > override;

      [[nodiscard]] auto v_is_valid( connection& c ) const noexcept -> bool override
      {
         return c.is_open();
      }

      // pass-key idiom
      class private_key final
      {
         private_key() = default;
         friend class connection_pool;
      };

   public:
      connection_pool( const private_key /*unused*/, const std::string_view connection_info );

      [[nodiscard]] static auto create( const std::string_view connection_info ) -> std::shared_ptr< connection_pool >;

      [[nodiscard]] auto connection() -> std::shared_ptr< connection >;

      template< typename... As >
      auto execute( const internal::zsv statement, As&&... as )
      {
         return connection()->direct()->execute( statement, std::forward< As >( as )... );
      }
   };

}  // namespace tao::pq

#endif
