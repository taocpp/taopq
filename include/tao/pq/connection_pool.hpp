// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_CONNECTION_POOL_HPP
#define TAO_PQ_CONNECTION_POOL_HPP

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/poll.hpp>
#include <tao/pq/internal/pool.hpp>
#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/parameter.hpp>
#include <tao/pq/poll.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   class connection_pool
      : public internal::pool< connection >
   {
   private:
      const std::string m_connection_info;
      std::optional< std::chrono::milliseconds > m_timeout;
      std::function< poll::callback > m_poll;

   protected:
      [[nodiscard]] auto v_create() const -> std::unique_ptr< pq::connection > override;

      [[nodiscard]] auto v_is_valid( connection& c ) const noexcept -> bool override
      {
         return c.is_idle();
      }

   private:
      // pass-key idiom
      class private_key final
      {
         private_key() = default;
         friend class connection_pool;
      };

   public:
      connection_pool( const private_key /*unused*/, const std::string_view connection_info, std::function< poll::callback > poll_cb );

      template< typename T = connection_pool >
      [[nodiscard]] static auto create( const std::string_view connection_info, std::function< poll::callback > poll_cb = internal::poll ) -> std::shared_ptr< T >
      {
         return std::make_shared< T >( private_key(), connection_info, std::move( poll_cb ) );
      }

      [[nodiscard]] auto timeout() const noexcept -> decltype( auto )
      {
         return m_timeout;
      }

      void set_timeout( const std::chrono::milliseconds timeout ) noexcept;
      void reset_timeout() noexcept;

      [[nodiscard]] auto poll_callback() const noexcept -> const std::function< poll::callback >&;
      void set_poll_callback( std::function< poll::callback > poll_cb ) noexcept;
      void reset_poll_callback();

      [[nodiscard]] auto connection() -> std::shared_ptr< connection >;

      template< parameter_type... As >
      auto execute( const internal::zsv statement, As&&... as )
      {
         return connection()->direct()->execute( statement, std::forward< As >( as )... );
      }
   };

}  // namespace tao::pq

#endif
