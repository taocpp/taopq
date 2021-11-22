// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_AGGREGATE_HPP
#define TAO_PQ_INTERNAL_AGGREGATE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

namespace tao::pq::internal::aggregate
{
   struct any
   {
      template< typename T >
      constexpr operator T() const noexcept;
   };

   template< std::size_t >
   using indexed_any = any;

   template< typename, typename, typename = void >
   inline constexpr bool check = false;

   template< typename T, std::size_t... Is >
   inline constexpr bool check< T, std::index_sequence< Is... >, decltype( (void)T{ std::declval< indexed_any< Is > >()... } ) > = true;

   template< typename T, std::size_t N >
   inline constexpr bool checkN = check< T, std::make_index_sequence< N > >;

   template< typename T, std::size_t N = 1, bool = checkN< T, N > >
   inline constexpr std::size_t minimum = N;

   template< typename T, std::size_t N >
   inline constexpr std::size_t minimum< T, N, false > = minimum< T, N + 1 >;

   template< typename T, std::size_t N = minimum< T >, bool = checkN< T, N > >
   inline constexpr std::size_t count = count< T, N + 1 >;

   template< typename T, std::size_t N >
   inline constexpr std::size_t count< T, N, false > = N - 1;

   template< typename T >
   constexpr auto tie( const T& t ) noexcept
   {
      static_assert( std::is_aggregate_v< T > );
      constexpr auto N = count< T >;
      if constexpr( N == 1 ) {
         const auto& [ a ] = t;
         return std::tie( a );
      }
      else if constexpr( N == 2 ) {
         const auto& [ a, b ] = t;
         return std::tie( a, b );
      }
      else if constexpr( N == 3 ) {
         const auto& [ a, b, c ] = t;
         return std::tie( a, b, c );
      }
      else if constexpr( N == 4 ) {
         const auto& [ a, b, c, d ] = t;
         return std::tie( a, b, c, d );
      }
      else if constexpr( N == 5 ) {
         const auto& [ a, b, c, d, e ] = t;
         return std::tie( a, b, c, d, e );
      }
      else if constexpr( N == 6 ) {
         const auto& [ a, b, c, d, e, f ] = t;
         return std::tie( a, b, c, d, e, f );
      }
      // TODO: more...
   }

}  // namespace tao::pq::internal::aggregate

#endif
