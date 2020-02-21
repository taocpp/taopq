// Copyright (c) 2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_ENDIAN_GCC_HPP
#define TAO_PQ_INTERNAL_ENDIAN_GCC_HPP

namespace tao::pq::internal
{
#if !defined( __BYTE_ORDER__ )
#error No byte order defined!
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

   [[nodiscard]] constexpr unsigned short hton( const unsigned short v ) noexcept
   {
      return v;
   }

   [[nodiscard]] constexpr unsigned int hton( const unsigned int v ) noexcept
   {
      return v;
   }

   [[nodiscard]] constexpr unsigned long hton( const unsigned long v ) noexcept
   {
      return v;
   }

   [[nodiscard]] constexpr unsigned long long hton( const unsigned long long v ) noexcept
   {
      return v;
   }

#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

   [[nodiscard]] constexpr unsigned short hton( const unsigned short v ) noexcept
   {
      return __builtin_bswap16( v );
   }

   [[nodiscard]] constexpr unsigned int hton( const unsigned int v ) noexcept
   {
      return __builtin_bswap32( v );
   }

   [[nodiscard]] constexpr unsigned long hton( const unsigned long v ) noexcept
   {
      static_assert( ( sizeof( unsigned long ) == 4 ) || ( sizeof( unsigned long ) == 8 ) );
      if constexpr( sizeof( unsigned long ) == 4 ) {
         return __builtin_bswap32( v );
      }
      else if constexpr( sizeof( unsigned long ) == 8 ) {
         return __builtin_bswap64( v );
      }
      else {
         return 0;
      }
   }

   [[nodiscard]] constexpr unsigned long long hton( const unsigned long long v ) noexcept
   {
      return __builtin_bswap64( v );
   }

#else
#error Unknown host byte order!
#endif

}  // namespace tao::pq::internal

#endif
