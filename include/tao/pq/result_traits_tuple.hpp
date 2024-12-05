// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_TRAITS_TUPLE_HPP
#define TAO_PQ_RESULT_TRAITS_TUPLE_HPP

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/exclusive_scan.hpp>
#include <tao/pq/result_traits.hpp>

template<>
struct tao::pq::result_traits< std::tuple<> >
{
   static constexpr std::size_t size = 0;
};

template< typename T >
struct tao::pq::result_traits< std::tuple< T > >
{
   static constexpr std::size_t size = result_traits_size< T >;

   template< typename U = T >
      requires std::is_same_v< T, U > && requires { result_traits< T >::null(); }
   [[nodiscard]] static auto null()
      -> std::tuple< T >
   {
      return std::tuple< T >( result_traits< T >::null() );
   }

   [[nodiscard]] static auto from( const char* value )
   {
      return std::tuple< T >( result_traits< T >::from( value ) );
   }
};

template< typename... Ts >
   requires( sizeof...( Ts ) >= 2 )
struct tao::pq::result_traits< std::tuple< Ts... > >
{
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
