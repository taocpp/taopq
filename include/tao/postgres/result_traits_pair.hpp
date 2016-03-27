// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_POSTGRES_RESULT_TRAITS_PAIR_HPP
#define TAOCPP_INCLUDE_POSTGRES_RESULT_TRAITS_PAIR_HPP

#include <utility>
#include <type_traits>
#include <tao/postgres/result_traits.hpp>
#include <tao/postgres/row.hpp>

namespace tao
{
  namespace postgres
  {
    template< typename T, typename U >
    struct result_traits< std::pair< T, U > >
    {
      using DT = typename std::decay< T >::type;
      using DU = typename std::decay< U >::type;

      static constexpr std::size_t size = result_traits_size< DT >::value + result_traits_size< DU >::value;

      static std::pair< T, U > from( const row& row )
      {
        return std::pair< T, U >( row.get< DT >( 0 ), row.get< DU >( result_traits_size< DT >::value ) );
      }
    };
  }
}

#endif // TAOCPP_INCLUDE_POSTGRES_RESULT_TRAITS_PAIR_HPP
