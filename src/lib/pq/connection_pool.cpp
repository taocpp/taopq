// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/connection_pool.hpp>

namespace tao::pq
{
   auto connection_pool::v_create() const -> std::unique_ptr< pq::connection >
   {
      return std::make_unique< pq::connection >( pq::connection::private_key(), m_connection_info );
   }

   connection_pool::connection_pool( const private_key /*unused*/, const std::string_view connection_info )
      : m_connection_info( connection_info )
   {}

   auto connection_pool::create( const std::string_view connection_info ) -> std::shared_ptr< connection_pool >
   {
      return std::make_shared< connection_pool >( private_key(), connection_info );
   }

   void connection_pool::set_timeout( const std::chrono::milliseconds timeout )
   {
      m_timeout = timeout;
   }

   void connection_pool::reset_timeout() noexcept
   {
      m_timeout = std::nullopt;
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
      return result;
   }

}  // namespace tao::pq
