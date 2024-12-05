// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_IS_ARRAY_HPP
#define TAO_PQ_IS_ARRAY_HPP

#include <array>
#include <cstddef>
#include <list>
#include <set>
#include <span>
#include <unordered_set>
#include <utility>
#include <vector>

namespace tao::pq
{
   namespace internal
   {
      template< typename >
      inline constexpr bool is_array_parameter = false;

      template< typename T, std::size_t N >
      inline constexpr bool is_array_parameter< std::span< T, N > > = true;

      template< std::size_t N >
      inline constexpr bool is_array_parameter< std::span< std::byte, N > > = false;

      template< std::size_t N >
      inline constexpr bool is_array_parameter< std::span< const std::byte, N > > = false;

      template< typename T, std::size_t N >
      inline constexpr bool is_array_parameter< std::array< T, N > > = true;

      template< typename... Ts >
      inline constexpr bool is_array_parameter< std::list< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array_parameter< std::set< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array_parameter< std::unordered_set< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array_parameter< std::vector< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array_parameter< std::vector< std::byte, Ts... > > = false;

      template< typename >
      inline constexpr bool is_array_result = false;

      template< typename... Ts >
      inline constexpr bool is_array_result< std::list< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array_result< std::set< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array_result< std::unordered_set< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array_result< std::vector< Ts... > > = true;

      template<>
      inline constexpr bool is_array_result< std::vector< std::byte > > = false;

   }  // namespace internal

   template< typename T >
   inline constexpr bool is_array_parameter = internal::is_array_parameter< T >;

   template< typename T >
   inline constexpr bool is_array_result = internal::is_array_result< T >;

}  // namespace tao::pq

#endif
