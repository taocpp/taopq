// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_AGGREGATE_HPP
#define TAO_PQ_INTERNAL_AGGREGATE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

namespace tao::pq::internal
{
   struct convert_to_any
   {
      template< typename T >
      constexpr operator T() const noexcept;
   };

   template< std::size_t >
   using indexed_convert_to_any = convert_to_any;

   template< typename, typename, typename = void >
   inline constexpr bool check_aggregate_args_impl = false;

   template< typename T, std::size_t... Is >
   inline constexpr bool check_aggregate_args_impl< T, std::index_sequence< Is... >, decltype( (void)T{ std::declval< indexed_convert_to_any< Is > >()... } ) > = true;

   template< typename T, std::size_t N >
   inline constexpr bool check_aggregate_args = check_aggregate_args_impl< T, std::make_index_sequence< N > >;

   template< typename T, std::size_t N = 1, bool = check_aggregate_args< T, N > >
   inline constexpr std::size_t minimum_aggregate_args = N;

   template< typename T, std::size_t N >
   inline constexpr std::size_t minimum_aggregate_args< T, N, false > = minimum_aggregate_args< T, N + 1 >;

   template< typename T, std::size_t N = minimum_aggregate_args< T >, bool = check_aggregate_args< T, N > >
   inline constexpr std::size_t count_aggregate_args = count_aggregate_args< T, N + 1 >;

   template< typename T, std::size_t N >
   inline constexpr std::size_t count_aggregate_args< T, N, false > = N - 1;

   template< typename T >
   constexpr auto tie_aggregate( const T& value ) noexcept
   {
      static_assert( std::is_aggregate_v< T > );
      static_assert( !std::is_empty_v< T > );
      static_assert( !std::is_union_v< T > );
      if constexpr( !std::is_aggregate_v< T > || std::is_empty_v< T > || std::is_union_v< T > ) {
         return std::tuple<>();
      }
      else {
         constexpr auto cnt = count_aggregate_args< T >;
         if constexpr( cnt == 1 ) {
            const auto& [ a ] = value;
            return std::tie( a );
         }
         else if constexpr( cnt == 2 ) {
            const auto& [ a, b ] = value;
            return std::tie( a, b );
         }
         else if constexpr( cnt == 3 ) {
            const auto& [ a, b, c ] = value;
            return std::tie( a, b, c );
         }
         else if constexpr( cnt == 4 ) {
            const auto& [ a, b, c, d ] = value;
            return std::tie( a, b, c, d );
         }
         else if constexpr( cnt == 5 ) {
            const auto& [ a, b, c, d, e ] = value;
            return std::tie( a, b, c, d, e );
         }
         else if constexpr( cnt == 6 ) {
            const auto& [ a, b, c, d, e, f ] = value;
            return std::tie( a, b, c, d, e, f );
         }
         else if constexpr( cnt == 7 ) {
            const auto& [ a, b, c, d, e, f, g ] = value;
            return std::tie( a, b, c, d, e, f, g );
         }
         else if constexpr( cnt == 8 ) {
            const auto& [ a, b, c, d, e, f, g, h ] = value;
            return std::tie( a, b, c, d, e, f, g, h );
         }
         else if constexpr( cnt == 9 ) {
            const auto& [ a, b, c, d, e, f, g, h, i ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i );
         }
         else if constexpr( cnt == 10 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j );
         }
         else if constexpr( cnt == 11 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k );
         }
         else if constexpr( cnt == 12 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l );
         }
         else if constexpr( cnt == 13 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m );
         }
         else if constexpr( cnt == 14 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n );
         }
         else if constexpr( cnt == 15 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o );
         }
         else if constexpr( cnt == 16 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p );
         }
         else if constexpr( cnt == 17 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q );
         }
         else if constexpr( cnt == 18 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r );
         }
         else if constexpr( cnt == 19 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s );
         }
         else if constexpr( cnt == 20 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t );
         }
         else if constexpr( cnt == 21 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u );
         }
         else if constexpr( cnt == 22 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v );
         }
         else if constexpr( cnt == 23 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w );
         }
         else if constexpr( cnt == 24 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x );
         }
         else if constexpr( cnt == 25 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y );
         }
         else if constexpr( cnt == 26 ) {
            const auto& [ a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z ] = value;
            return std::tie( a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z );
         }
         // TODO: more...
      }
   }

}  // namespace tao::pq::internal

#endif
