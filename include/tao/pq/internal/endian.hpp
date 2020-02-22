// Copyright (c) 2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_ENDIAN_HPP
#define TAO_PQ_INTERNAL_ENDIAN_HPP

#include <cstdint>
#include <cstring>
#include <type_traits>

#if defined( _WIN32 ) && !defined( __MINGW32__ ) && !defined( __CYGWIN__ )
#include "endian_win.hpp"
#else
#include "endian_gcc.hpp"
#endif

namespace tao::pq::internal
{
   [[nodiscard]] constexpr char hton( const char v ) noexcept
   {
      return v;
   }

   [[nodiscard]] constexpr signed char hton( const signed char v ) noexcept
   {
      return v;
   }

   [[nodiscard]] constexpr unsigned char hton( const unsigned char v ) noexcept
   {
      return v;
   }

   template< typename U, typename V >
   [[nodiscard]] V hton_impl( V v ) noexcept
   {
      static_assert( sizeof( U ) == sizeof( V ) );
      static_assert( std::is_trivial_v< U > );
      static_assert( std::is_trivial_v< V > );

      U u;
      std::memcpy( &u, &v, sizeof( U ) );
      u = internal::hton( u );
      std::memcpy( &v, &u, sizeof( U ) );
      return v;
   }

   [[nodiscard]] inline short hton( const short v ) noexcept
   {
      return hton_impl< std::uint16_t >( v );
   }

   [[nodiscard]] inline int hton( const int v ) noexcept
   {
      return hton_impl< std::uint32_t >( v );
   }

   [[nodiscard]] inline long hton( const long v ) noexcept
   {
      static_assert( ( sizeof( long ) == 4 ) || ( sizeof( long ) == 8 ) );
      return hton_impl< std::conditional_t< sizeof( long ) == 4, std::uint32_t, std::uint64_t > >( v );
   }

   [[nodiscard]] inline long long hton( const long long v ) noexcept
   {
      return hton_impl< std::uint64_t >( v );
   }

   [[nodiscard]] inline float hton( const float v ) noexcept
   {
      return hton_impl< std::uint32_t >( v );
   }

   [[nodiscard]] inline double hton( const double v ) noexcept
   {
      return hton_impl< std::uint64_t >( v );
   }

}  // namespace tao::pq::internal

#endif
