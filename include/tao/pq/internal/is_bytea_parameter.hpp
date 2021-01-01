// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_IS_BYTEA_PARAMETER_HPP
#define TAO_PQ_INTERNAL_IS_BYTEA_PARAMETER_HPP

#include <cstddef>
#include <type_traits>

namespace tao::pq::internal
{
   // clang-format off
   template< typename > inline constexpr bool is_bytea_parameter = false;
   template< typename T > inline constexpr bool is_bytea_parameter< const T > = is_bytea_parameter< T >;

   template<> inline constexpr bool is_bytea_parameter< char > = true;
   template<> inline constexpr bool is_bytea_parameter< signed char > = true;
   template<> inline constexpr bool is_bytea_parameter< unsigned char > = true;
   template<> inline constexpr bool is_bytea_parameter< std::byte > = true;
   // clang-format on

}  // namespace tao::pq::internal

#endif
