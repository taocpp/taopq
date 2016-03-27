// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_POSTGRES_RESULT_TRAITS_TUPLE_HPP
#define TAOCPP_INCLUDE_POSTGRES_RESULT_TRAITS_TUPLE_HPP

#include <tuple>
#include <type_traits>
#include <utility>
#include <tao/seq/sum.hpp>
#include <tao/seq/exclusive_scan.hpp>
#include <tao/postgres/result_traits.hpp>
#include <tao/postgres/row.hpp>

namespace tao
{
  namespace postgres
  {
    template< typename... Ts >
    struct result_traits< std::tuple< Ts... >, typename std::enable_if< sizeof...( Ts ) == 1 >::type >
    {
      using T = typename std::tuple_element< 0, std::tuple< Ts... > >::type;
      static constexpr std::size_t size = result_traits_size< T >::value;

      static typename std::enable_if< result_traits_has_null< T >::value, std::tuple< T > >::type null()
      {
        return std::tuple< T >( result_traits< T >::null() );
      }

      static std::tuple< T > from( const char* value )
      {
        return std::tuple< T >( result_traits< T >::from( value ) );
      }
    };

    template< typename... Ts >
    struct result_traits< std::tuple< Ts... >, typename std::enable_if< ( sizeof...( Ts ) > 1 ) >::type >
    {
      static constexpr std::size_t size = seq::sum< std::size_t, result_traits_size< Ts >::value... >::value;

      template< std::size_t... Ns >
      static std::tuple< Ts... > from( const row& row, const seq::index_sequence< Ns... >& )
      {
        return std::tuple< Ts... >( row.get< Ts >( Ns )... );
      }

      static std::tuple< Ts... > from( const row& row )
      {
        return from( row, seq::exclusive_scan_t< std::size_t, result_traits_size< Ts >::value... >() );
      }
    };
  }
}

#endif // TAOCPP_INCLUDE_POSTGRES_RESULT_TRAITS_TUPLE_HPP
