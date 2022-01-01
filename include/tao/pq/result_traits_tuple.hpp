// Copyright (c) 2016-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_TRAITS_TUPLE_HPP
#define TAO_PQ_RESULT_TRAITS_TUPLE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/exclusive_scan.hpp>
#include <tao/pq/result_traits.hpp>

template< typename T >
struct tao::pq::result_traits< std::tuple< T > >
{
   static constexpr std::size_t size = result_traits_size< T >;

   template< typename U = T >
   [[nodiscard]] static auto null()
      -> std::enable_if_t< std::is_same_v< T, U > && result_traits_has_null< T >, std::tuple< T > >
   {
      return std::tuple< T >( result_traits< T >::null() );
   }

   [[nodiscard]] static auto from( const char* value )
   {
      return std::tuple< T >( result_traits< T >::from( value ) );
   }
};

template< typename... Ts >
struct tao::pq::result_traits< std::tuple< Ts... > >
{
   static_assert( sizeof...( Ts ) != 0, "conversion to empty std::tuple<> not support" );

   static constexpr std::size_t size = ( 0 + ... + result_traits_size< Ts > );

   template< typename Row, std::size_t... Ns >
   [[nodiscard]] static auto from( const Row& row, std::index_sequence< Ns... > /*unused*/ )
   {
      return std::tuple< Ts... >( row.template get< Ts >( Ns )... );
   }

   template< typename Row >
   [[nodiscard]] static auto from( const Row& row )
   {
      return result_traits::from( row, internal::exclusive_scan_t< std::index_sequence< result_traits_size< Ts >... > >() );
   }
};

#endif
