// Copyright (c) 2024-2025 Daniel Frey and Dr. Colin Hirsch
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
      inline constexpr bool is_array = false;

      template< typename... Ts >
      inline constexpr bool is_array< std::list< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array< std::set< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array< std::unordered_set< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array< std::vector< Ts... > > = true;

      template< typename... Ts >
      inline constexpr bool is_array< std::vector< std::byte, Ts... > > = false;

   }  // namespace internal

   template< typename T >
   inline constexpr bool is_array = internal::is_array< T >;

   namespace internal
   {
      template< typename T >
      inline constexpr bool is_array_parameter = pq::is_array< T >;

      template< typename T, std::size_t N >
      inline constexpr bool is_array_parameter< std::span< T, N > > = true;

      template< std::size_t N >
      inline constexpr bool is_array_parameter< std::span< std::byte, N > > = false;

      template< std::size_t N >
      inline constexpr bool is_array_parameter< std::span< const std::byte, N > > = false;

      template< typename T, std::size_t N >
      inline constexpr bool is_array_parameter< std::array< T, N > > = true;

   }  // namespace internal

   template< typename T >
   inline constexpr bool is_array_parameter = internal::is_array_parameter< T >;

   namespace internal
   {
      template< typename T >
      inline constexpr bool is_array_result = pq::is_array< T >;

   }  // namespace internal

   template< typename T >
   inline constexpr bool is_array_result = internal::is_array_result< T >;

}  // namespace tao::pq

#endif
