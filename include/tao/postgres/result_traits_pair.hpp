// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_RESULT_TRAITS_PAIR_HPP
#define TAO_POSTGRES_RESULT_TRAITS_PAIR_HPP

#include <tao/postgres/result_traits.hpp>
#include <tao/postgres/row.hpp>
#include <type_traits>
#include <utility>

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

   }  // namespace postgres

}  // namespace tao

#endif
