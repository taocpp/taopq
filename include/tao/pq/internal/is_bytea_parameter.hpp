// Copyright (c) 2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_IS_BYTEA_PARAMETER_HPP
#define TAO_PQ_INTERNAL_IS_BYTEA_PARAMETER_HPP

#include <cstddef>
#include <type_traits>

namespace tao::pq::internal
{
   // clang-format off
   template< typename > struct is_bytea_parameter : std::false_type {};
   template< typename T > struct is_bytea_parameter< const T > : is_bytea_parameter< T > {};

   template<> struct is_bytea_parameter< char > : std::true_type {};
   template<> struct is_bytea_parameter< signed char > : std::true_type {};
   template<> struct is_bytea_parameter< unsigned char > : std::true_type {};
   template<> struct is_bytea_parameter< std::byte > : std::true_type {};
   // clang-format on

}  // namespace tao::pq::internal

#endif
