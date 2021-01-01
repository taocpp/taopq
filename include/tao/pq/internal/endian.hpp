// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
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
   [[nodiscard]] constexpr auto hton( const char v ) noexcept -> char
   {
      return v;
   }

   [[nodiscard]] constexpr auto hton( const signed char v ) noexcept -> signed char
   {
      return v;
   }

   [[nodiscard]] constexpr auto hton( const unsigned char v ) noexcept -> unsigned char
   {
      return v;
   }

   template< typename U, typename V >
   [[nodiscard]] auto hton_impl( V v ) noexcept -> V
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

   [[nodiscard]] inline auto hton( const short v ) noexcept -> short
   {
      return hton_impl< std::uint16_t >( v );
   }

   [[nodiscard]] inline auto hton( const int v ) noexcept -> int
   {
      return hton_impl< std::uint32_t >( v );
   }

   [[nodiscard]] inline auto hton( const long v ) noexcept -> long
   {
      static_assert( ( sizeof( long ) == 4 ) || ( sizeof( long ) == 8 ) );
      return hton_impl< std::conditional_t< sizeof( long ) == 4, std::uint32_t, std::uint64_t > >( v );
   }

   [[nodiscard]] inline auto hton( const long long v ) noexcept -> long long
   {
      return hton_impl< std::uint64_t >( v );
   }

   [[nodiscard]] inline auto hton( const float v ) noexcept -> float
   {
      return hton_impl< std::uint32_t >( v );
   }

   [[nodiscard]] inline auto hton( const double v ) noexcept -> double
   {
      return hton_impl< std::uint64_t >( v );
   }

}  // namespace tao::pq::internal

#endif
