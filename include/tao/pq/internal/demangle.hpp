// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

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
      return internal::demangle( typeid( T ) );
   }

}  // namespace tao::pq::internal

#endif
