// Copyright (c) 2015-2018 Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/sequences/

#ifndef TAO_SEQ_EXCLUSIVE_SCAN_HPP
#define TAO_SEQ_EXCLUSIVE_SCAN_HPP

#include <utility>

#include "make_integer_sequence.hpp"
#include "partial_sum.hpp"

namespace tao
{
   namespace seq
   {
      namespace impl
      {
         template< typename S, typename = make_index_sequence< S::size() > >
         struct exclusive_scan;

         template< typename S, std::size_t... Is >
         struct exclusive_scan< S, index_sequence< Is... > >
         {
            using type = integer_sequence< typename S::value_type, partial_sum< Is, S >::value... >;
         };

      }  // namespace impl

      template< typename T, T... Ns >
      struct exclusive_scan
         : impl::exclusive_scan< integer_sequence< T, Ns... > >
      {
      };

      template< typename T, T... Ns >
      struct exclusive_scan< integer_sequence< T, Ns... > >
         : impl::exclusive_scan< integer_sequence< T, Ns... > >
      {
      };

      template< typename T, T... Ns >
      using exclusive_scan_t = typename exclusive_scan< T, Ns... >::type;

   }  // namespace seq

}  // namespace tao

#endif
