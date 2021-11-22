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
      else if constexpr( N == 7 ) {
         const auto& [ a, b, c, d, e, f, g ] = t;
         return std::tie( a, b, c, d, e, f, g );
      }
      else if constexpr( N == 8 ) {
         const auto& [ a, b, c, d, e, f, g, h ] = t;
         return std::tie( a, b, c, d, e, f, g, h );
      }
      else if constexpr( N == 9 ) {
         const auto& [ a, b, c, d, e, f, g, h, i ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i );
      }
      else if constexpr( N == 10 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j );
      }
      else if constexpr( N == 11 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k );
      }
      else if constexpr( N == 12 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l );
      }
      else if constexpr( N == 13 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m );
      }
      else if constexpr( N == 14 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n );
      }
      else if constexpr( N == 15 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o );
      }
      else if constexpr( N == 16 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p );
      }
      else if constexpr( N == 17 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q );
      }
      else if constexpr( N == 18 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r );
      }
      else if constexpr( N == 19 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s );
      }
      else if constexpr( N == 20 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t );
      }
      else if constexpr( N == 21 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u );
      }
      else if constexpr( N == 22 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v );
      }
      else if constexpr( N == 23 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w );
      }
      else if constexpr( N == 24 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x );
      }
      else if constexpr( N == 25 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y );
      }
      else if constexpr( N == 26 ) {
         const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z ] = t;
         return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z );
      }
      // TODO: more...
   }

}  // namespace tao::pq::internal::aggregate

#endif
