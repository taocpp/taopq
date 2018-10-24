// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_RESULT_TRAITS_TUPLE_HPP
#define TAO_POSTGRES_RESULT_TRAITS_TUPLE_HPP

#include <tao/postgres/result_traits.hpp>
#include <tao/postgres/row.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

namespace tao
{
   namespace postgres
   {
      namespace impl
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

      }  // namespace impl

      template< typename... Ts >
      struct result_traits< std::tuple< Ts... >, std::enable_if_t< sizeof...( Ts ) == 1 > >
      {
         using T = std::tuple_element_t< 0, std::tuple< Ts... > >;
         static constexpr std::size_t size = result_traits_size< T >;

         static std::enable_if_t< result_traits_has_null< T >, std::tuple< T > > null()
         {
            return std::tuple< T >( result_traits< T >::null() );
         }

         static std::tuple< T > from( const char* value )
         {
            return std::tuple< T >( result_traits< T >::from( value ) );
         }
      };

      template< typename... Ts >
      struct result_traits< std::tuple< Ts... >, std::enable_if_t< ( sizeof...( Ts ) > 1 ) > >
      {
         static constexpr std::size_t size = ( result_traits_size< Ts > + ... );

         template< std::size_t... Ns >
         static std::tuple< Ts... > from( const row& row, const std::index_sequence< Ns... >& )
         {
            return std::tuple< Ts... >( row.get< Ts >( Ns )... );
         }

         static std::tuple< Ts... > from( const row& row )
         {
            return from( row, impl::exclusive_scan_t< result_traits_size< Ts >... >() );
         }
      };

   }  // namespace postgres

}  // namespace tao

#endif
