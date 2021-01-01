// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_ENDIAN_GCC_HPP
#define TAO_PQ_INTERNAL_ENDIAN_GCC_HPP

namespace tao::pq::internal
{
#if !defined( __BYTE_ORDER__ )
#error No byte order defined!
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

   [[nodiscard]] constexpr auto hton( const unsigned short v ) noexcept -> unsigned short
   {
      return v;
   }

   [[nodiscard]] constexpr auto hton( const unsigned int v ) noexcept -> unsigned int
   {
      return v;
   }

   [[nodiscard]] constexpr auto hton( const unsigned long v ) noexcept -> unsigned long
   {
      return v;
   }

   [[nodiscard]] constexpr auto hton( const unsigned long long v ) noexcept -> unsigned long long
   {
      return v;
   }

#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

   [[nodiscard]] inline auto hton( const unsigned short v ) noexcept -> unsigned short
   {
      return __builtin_bswap16( v );
   }

   [[nodiscard]] inline auto hton( const unsigned int v ) noexcept -> unsigned int
   {
      return __builtin_bswap32( v );
   }

   [[nodiscard]] inline auto hton( const unsigned long v ) noexcept -> unsigned long
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

   [[nodiscard]] inline auto hton( const unsigned long long v ) noexcept -> unsigned long long
   {
      return __builtin_bswap64( v );
   }

#else
#error Unknown host byte order!
#endif

}  // namespace tao::pq::internal

#endif
