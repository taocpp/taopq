// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/connection_pool.hpp>

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/poll.hpp>
#include <tao/pq/poll.hpp>

namespace tao::pq
{
   auto connection_pool::v_create() const -> std::unique_ptr< pq::connection >
   {
      return std::make_unique< pq::connection >( pq::connection::private_key(), m_connection_info, m_poll );
   }

   connection_pool::connection_pool( const private_key /*unused*/, const std::string_view connection_info, std::function< poll::callback > poll_cb )
      : m_connection_info( connection_info ),
        m_poll( std::move( poll_cb ) )
   {}

   void connection_pool::set_timeout( const std::chrono::milliseconds timeout ) noexcept
   {
      m_timeout = timeout;
   }

   void connection_pool::reset_timeout() noexcept
   {
      m_timeout = std::nullopt;
   }

   auto connection_pool::poll_callback() const noexcept -> const std::function< poll::callback >&
   {
      return m_poll;
   }

   void connection_pool::set_poll_callback( std::function< poll::callback > poll_cb ) noexcept
   {
      m_poll = std::move( poll_cb );
   }

   void connection_pool::reset_poll_callback()
   {
      m_poll = internal::poll;
   }

   auto connection_pool::connection() -> std::shared_ptr< pq::connection >
   {
      auto result = get();
      if( m_timeout ) {
         result->set_timeout( *m_timeout );
      }
      else {
         result->reset_timeout();
      }
      result->set_poll_callback( m_poll );
      return result;
   }

}  // namespace tao::pq
