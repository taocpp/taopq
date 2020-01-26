// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/connection_pool.hpp>

namespace tao::pq
{
   std::unique_ptr< connection > connection_pool::v_create() const
   {
      return std::make_unique< pq::connection >( connection::private_key(), m_connection_info );
   }

   bool connection_pool::v_is_valid( pq::connection& c ) const noexcept
   {
      return c.is_open();
   }

   std::shared_ptr< connection_pool > connection_pool::create( const std::string& connection_info )
   {
      return std::make_shared< connection_pool >( connection_pool::private_key(), connection_info );
   }

}  // namespace tao::pq
