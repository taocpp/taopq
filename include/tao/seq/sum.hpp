// Copyright (c) 2015-2018 Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/sequences/

#ifndef TAO_SEQ_SUM_HPP
#define TAO_SEQ_SUM_HPP

#include <type_traits>
#include <utility>

namespace tao
{
   namespace seq
   {
      template< typename T, T... Ns >
      struct sum
         : std::integral_constant< T, ( Ns + ... + T( 0 ) ) >
      {
      };

      template< typename T, T... Ns >
      struct sum< std::integer_sequence< T, Ns... > >
         : sum< T, Ns... >
      {
      };

   }  // namespace seq

}  // namespace tao

#endif
