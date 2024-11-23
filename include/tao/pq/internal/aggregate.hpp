// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_AGGREGATE_HPP
#define TAO_PQ_INTERNAL_AGGREGATE_HPP

#include <cstddef>
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

   template< typename, typename >
   inline constexpr bool check_aggregate_args_impl = false;

   template< typename T, std::size_t... Is >
      requires requires { T{ std::declval< indexed_convert_to_any< Is > >()... }; }
   inline constexpr bool check_aggregate_args_impl< T, std::index_sequence< Is... > > = true;

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

#define TAO_PQ_TIE( N, ... )               \
   if constexpr( cnt == N ) {              \
      const auto& [ __VA_ARGS__ ] = value; \
      return std::tie( __VA_ARGS__ );      \
   }                                       \
   else                                    \
      static_assert( true )

#define TAO_PQ_10( P ) P##0, P##1, P##2, P##3, P##4, P##5, P##6, P##7, P##8, P##9
#define TAO_PQ_TIE10( N, ... )                               \
   TAO_PQ_TIE( N, __VA_ARGS__ );                             \
   TAO_PQ_TIE( N + 1, __VA_ARGS__, a );                      \
   TAO_PQ_TIE( N + 2, __VA_ARGS__, a, b );                   \
   TAO_PQ_TIE( N + 3, __VA_ARGS__, a, b, c );                \
   TAO_PQ_TIE( N + 4, __VA_ARGS__, a, b, c, d );             \
   TAO_PQ_TIE( N + 5, __VA_ARGS__, a, b, c, d, e );          \
   TAO_PQ_TIE( N + 6, __VA_ARGS__, a, b, c, d, e, f );       \
   TAO_PQ_TIE( N + 7, __VA_ARGS__, a, b, c, d, e, f, g );    \
   TAO_PQ_TIE( N + 8, __VA_ARGS__, a, b, c, d, e, f, g, h ); \
   TAO_PQ_TIE( N + 9, __VA_ARGS__, a, b, c, d, e, f, g, h, i );

   template< typename T >
      requires( std::is_aggregate_v< T > && !std::is_empty_v< T > && !std::is_union_v< T > )
   constexpr auto tie_aggregate( const T& value ) noexcept
   {
      constexpr auto cnt = count_aggregate_args< T >;

      TAO_PQ_TIE( 1, a );
      TAO_PQ_TIE( 2, a, b );
      TAO_PQ_TIE( 3, a, b, c );
      TAO_PQ_TIE( 4, a, b, c, d );
      TAO_PQ_TIE( 5, a, b, c, d, e );
      TAO_PQ_TIE( 6, a, b, c, d, e, f );
      TAO_PQ_TIE( 7, a, b, c, d, e, f, g );
      TAO_PQ_TIE( 8, a, b, c, d, e, f, g, h );
      TAO_PQ_TIE( 9, a, b, c, d, e, f, g, h, i );

      TAO_PQ_TIE10( 10, TAO_PQ_10( a ) );
      TAO_PQ_TIE10( 20, TAO_PQ_10( a ), TAO_PQ_10( b ) );
      TAO_PQ_TIE10( 30, TAO_PQ_10( a ), TAO_PQ_10( b ), TAO_PQ_10( c ) );
      TAO_PQ_TIE10( 40, TAO_PQ_10( a ), TAO_PQ_10( b ), TAO_PQ_10( c ), TAO_PQ_10( d ) );
      TAO_PQ_TIE10( 50, TAO_PQ_10( a ), TAO_PQ_10( b ), TAO_PQ_10( c ), TAO_PQ_10( d ), TAO_PQ_10( e ) );
      TAO_PQ_TIE10( 60, TAO_PQ_10( a ), TAO_PQ_10( b ), TAO_PQ_10( c ), TAO_PQ_10( d ), TAO_PQ_10( e ), TAO_PQ_10( f ) );
      TAO_PQ_TIE10( 70, TAO_PQ_10( a ), TAO_PQ_10( b ), TAO_PQ_10( c ), TAO_PQ_10( d ), TAO_PQ_10( e ), TAO_PQ_10( f ), TAO_PQ_10( g ) );
      TAO_PQ_TIE10( 80, TAO_PQ_10( a ), TAO_PQ_10( b ), TAO_PQ_10( c ), TAO_PQ_10( d ), TAO_PQ_10( e ), TAO_PQ_10( f ), TAO_PQ_10( g ), TAO_PQ_10( h ) );
      TAO_PQ_TIE10( 90, TAO_PQ_10( a ), TAO_PQ_10( b ), TAO_PQ_10( c ), TAO_PQ_10( d ), TAO_PQ_10( e ), TAO_PQ_10( f ), TAO_PQ_10( g ), TAO_PQ_10( h ), TAO_PQ_10( i ) );
   }

#undef TAO_PQ_TIE
#undef TAO_PQ_10
#undef TAO_PQ_TIE10

}  // namespace tao::pq::internal

#endif
