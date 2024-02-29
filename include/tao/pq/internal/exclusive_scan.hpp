// Copyright (c) 2019-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_EXCLUSIVE_SCAN_HPP
#define TAO_PQ_INTERNAL_EXCLUSIVE_SCAN_HPP

#include <cstddef>
#include <utility>

namespace tao::pq::internal
{
   template< typename S, typename = std::make_index_sequence< S::size() > >
   struct exclusive_scan;

   template< typename T, T... Ns, std::size_t... Is >
   struct exclusive_scan< std::integer_sequence< T, Ns... >, std::index_sequence< Is... > >
   {
      template< std::size_t I >
      static constexpr T partial_sum = ( T( 0 ) + ... + ( ( Is < I ) ? Ns : T( 0 ) ) );

      using type = std::integer_sequence< T, partial_sum< Is >... >;
   };

   template< typename S >
   using exclusive_scan_t = typename exclusive_scan< S >::type;

}  // namespace tao::pq::internal

#endif
