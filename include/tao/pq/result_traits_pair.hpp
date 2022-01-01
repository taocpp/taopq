// Copyright (c) 2016-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_TRAITS_PAIR_HPP
#define TAO_PQ_RESULT_TRAITS_PAIR_HPP

#include <type_traits>
#include <utility>

#include <tao/pq/result_traits.hpp>

template< typename T, typename U >
struct tao::pq::result_traits< std::pair< T, U > >
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

#endif
