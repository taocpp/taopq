// Copyright (c) 2024-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_FORMAT_AS_HPP
#define TAO_PQ_INTERNAL_FORMAT_AS_HPP

#include <format>
#include <ostream>
#include <utility>

template< typename T >
   requires requires { taopq_format_as( std::declval< T >() ); }
struct std::formatter< T > : std::formatter< decltype( taopq_format_as( std::declval< T >() ) ) >
{
   auto format( const T& v, auto& ctx ) const
   {
      return std::formatter< decltype( taopq_format_as( v ) ) >::format( taopq_format_as( v ), ctx );
   }
};

template< typename T >
   requires requires { taopq_format_as( std::declval< T >() ); }
auto operator<<( std::ostream& os, const T& v ) -> std::ostream&
{
   return os << taopq_format_as( v );
}

#endif
