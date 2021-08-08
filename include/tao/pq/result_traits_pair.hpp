// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_RESULT_TRAITS_PAIR_HPP
#define TAO_PQ_RESULT_TRAITS_PAIR_HPP

#include <type_traits>
#include <utility>

#include <tao/pq/result_traits.hpp>

namespace tao::pq
{
   template< typename T, typename U >
   struct result_traits< std::pair< T, U > >
   {
      using DT = std::decay_t< T >;
      using DU = std::decay_t< U >;

      static constexpr std::size_t size = result_traits_size< DT > + result_traits_size< DU >;

      template< typename Row >
      [[nodiscard]] static auto from( const Row& row )
      {
         return std::pair< T, U >( row.template get< DT >( 0 ), row.template get< DU >( result_traits_size< DT > ) );
      }
   };

}  // namespace tao::pq

#endif
