// Copyright (c) 2015-2018 Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/sequences/

#ifndef TAO_SEQ_SUM_HPP
#define TAO_SEQ_SUM_HPP

namespace tao
{
   namespace seq
   {
      template< typename T, T... Ns >
      inline constexpr T sum = ( Ns + ... );

   }  // namespace seq

}  // namespace tao

#endif
