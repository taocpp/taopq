// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_RESULT_TRAITS_TUPLE_HPP
#define TAO_PQ_RESULT_TRAITS_TUPLE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/exclusive_scan.hpp>
#include <tao/pq/result_traits.hpp>
#include <tao/pq/row.hpp>

namespace tao::pq
{
   template< typename T >
   struct result_traits< std::tuple< T > >
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
   struct result_traits< std::tuple< Ts... > >
   {
      static_assert( sizeof...( Ts ) != 0, "conversion to empty std::tuple<> not support" );

      static constexpr std::size_t size{ (0 + ... + result_traits_size< Ts >)};

      template< std::size_t... Ns >
      [[nodiscard]] static auto from( const row& row, std::index_sequence< Ns... > /*unused*/ )
      {
         return std::tuple< Ts... >( row.get< Ts >( Ns )... );
      }

      [[nodiscard]] static auto from( const row& row )
      {
         return from( row, internal::exclusive_scan_t< std::index_sequence< result_traits_size< Ts >... > >() );
      }
   };

}  // namespace tao::pq

#endif
