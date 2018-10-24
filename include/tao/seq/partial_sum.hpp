// Copyright (c) 2015-2018 Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/sequences/

#ifndef TAO_SEQ_PARTIAL_SUM_HPP
#define TAO_SEQ_PARTIAL_SUM_HPP

#include <cstddef>
#include <utility>

namespace tao
{
   namespace seq
   {
      namespace impl
      {
         template< std::size_t, typename S, typename = std::make_index_sequence< S::size() > >
         struct partial_sum;

         template< std::size_t I, typename T, T... Ns, std::size_t... Is >
         struct partial_sum< I, std::integer_sequence< T, Ns... >, std::index_sequence< Is... > >
            : std::integral_constant< T, ( ( ( Is < I ) ? Ns : 0 ) + ... ) >
         {
            static_assert( I <= sizeof...( Is ), "tao::seq::partial_sum<I, S>: I is out of range" );
         };

      }  // namespace impl

      template< std::size_t I, typename T, T... Ns >
      using partial_sum = impl::partial_sum< I, std::integer_sequence< T, Ns... > >;

   }  // namespace seq

}  // namespace tao

#endif
