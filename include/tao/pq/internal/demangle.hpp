// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_DEMANGLE_HPP
#define TAO_PQ_INTERNAL_DEMANGLE_HPP

#include <string>
#include <typeinfo>

namespace tao::pq::internal
{
   [[nodiscard]] auto demangle( const char* const symbol ) -> std::string;

   [[nodiscard]] inline auto demangle( const std::type_info& type_info )
   {
      return demangle( type_info.name() );
   }

   template< typename T >
   [[nodiscard]] auto demangle()
   {
      return demangle( typeid( T ) );
   }

}  // namespace tao::pq::internal

#endif
