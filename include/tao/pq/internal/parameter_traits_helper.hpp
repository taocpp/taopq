// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_PARAMETER_TRAITS_HELPER_HPP
#define TAO_PQ_INTERNAL_PARAMETER_TRAITS_HELPER_HPP

#include <cassert>
#include <charconv>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

#include <tao/pq/oid.hpp>

namespace tao::pq::internal
{
   class char_pointer_helper
   {
   protected:
      const char* const m_p;

   protected:
      explicit constexpr char_pointer_helper( const char* p ) noexcept
         : m_p( p )
      {}

   public:
      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] constexpr auto value() const noexcept -> const char*
      {
         return m_p;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] constexpr auto string_view() const noexcept -> std::string_view
      {
         return m_p;
      }
   };

   struct buffer_helper
   {
      char m_buffer[ 32 ];

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return m_buffer;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] auto string_view() const noexcept -> std::string_view
      {
         return m_buffer;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto escape() noexcept -> bool
      {
         return false;
      }
   };

   struct to_chars_helper
      : buffer_helper
   {
      template< typename T >
      explicit to_chars_helper( const T v ) noexcept
      {
         const auto [ ptr, ec ] = std::to_chars( std::begin( m_buffer ), std::end( m_buffer ), v );
         assert( ec == std::errc() );
         *ptr = '\0';
      }
   };

}  // namespace tao::pq::internal

#endif
