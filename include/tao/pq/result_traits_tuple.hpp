// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_RESULT_TRAITS_TUPLE_HPP
#define TAO_PQ_RESULT_TRAITS_TUPLE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/result_traits.hpp>
#include <tao/pq/row.hpp>

namespace tao::pq
{
   namespace internal
   {
      template< typename, std::size_t... >
      struct exclusive_scan;

      template< std::size_t... Is, std::size_t... Ns >
      struct exclusive_scan< std::index_sequence< Is... >, Ns... >
      {
         template< std::size_t I >
         static constexpr std::size_t partial_sum = ( ( ( Is < I ) ? Ns : 0 ) + ... );

         using type = std::index_sequence< partial_sum< Is >... >;
      };

      template< std::size_t... Ns >
      using exclusive_scan_t = typename exclusive_scan< std::make_index_sequence< sizeof...( Ns ) >, Ns... >::type;

   }  // namespace internal

   template< typename T >
   struct result_traits< std::tuple< T > >
   {
      static constexpr std::size_t size = result_traits_size< T >;

      template< typename U = T >
      [[nodiscard]] static std::enable_if_t< std::is_same_v< T, U > && result_traits_has_null< T >, std::tuple< T > > null()
      {
         return std::tuple< T >( result_traits< T >::null() );
      }

      [[nodiscard]] static std::tuple< T > from( const char* value )
      {
         return std::tuple< T >( result_traits< T >::from( value ) );
      }
   };

   template< typename... Ts >
   struct result_traits< std::tuple< Ts... > >
   {
      static_assert( sizeof...( Ts ) != 0, "conversion to empty std::tuple<> not support" );

      static constexpr std::size_t size{ ( result_traits_size< Ts > + ... + 0 ) };

      template< std::size_t... Ns >
      [[nodiscard]] static std::tuple< Ts... > from( const row& row, std::index_sequence< Ns... > )
      {
         return std::tuple< Ts... >( row.get< Ts >( Ns )... );
      }

      [[nodiscard]] static std::tuple< Ts... > from( const row& row )
      {
         return from( row, internal::exclusive_scan_t< result_traits_size< Ts >... >() );
      }
   };

}  // namespace tao::pq

#endif
