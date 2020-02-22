// Copyright (c) 2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_ENDIAN_WIN_HPP
#define TAO_PQ_INTERNAL_ENDIAN_WIN_HPP

#include <cstdlib>

namespace tao::pq::internal
{
   // We assume that all Windows systems are little endian.

   [[nodiscard]] inline unsigned short hton( const unsigned short v ) noexcept
   {
      return _byteswap_ushort( v );
   }

   [[nodiscard]] inline unsigned int hton( const unsigned int v ) noexcept
   {
      return _byteswap_ulong( v );
   }

   [[nodiscard]] inline unsigned long hton( const unsigned long v ) noexcept
   {
      return _byteswap_ulong( v );
   }

   [[nodiscard]] inline unsigned long long hton( const unsigned long long v ) noexcept
   {
      return _byteswap_uint64( v );
   }

}  // namespace tao::pq::internal

#endif
