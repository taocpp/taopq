// Copyright (c) 2021-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/connection_pool.hpp>

#include <memory>
#include <string_view>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/poll.hpp>

namespace tao::pq
{
   auto connection_pool::v_create() const -> std::unique_ptr< pq::connection >
   {
      return std::make_unique< pq::connection >( pq::connection::private_key(), m_connection_info );
   }

   connection_pool::connection_pool( const private_key /*unused*/, const std::string_view connection_info )
      : m_connection_info( connection_info ),
        m_poll( internal::poll )
   {}

   auto connection_pool::connection() -> std::shared_ptr< pq::connection >
   {
      auto result = internal::pool< pq::connection >::get();
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
