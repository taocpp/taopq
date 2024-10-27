// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_PARAMETER_TRAITS_HELPER_HPP
#define TAO_PQ_INTERNAL_PARAMETER_TRAITS_HELPER_HPP

#include <cassert>
#include <charconv>
#include <cstddef>
#include <string>

#include <tao/pq/oid.hpp>

namespace tao::pq::internal
{
   struct char_pointer_helper
   {
   protected:
      const char* const m_p;

   public:
      explicit char_pointer_helper( const char* p ) noexcept
         : m_p( p )
      {}

      static constexpr std::size_t columns = 1;
      static constexpr bool self_contained = false;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return oid::invalid;
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
   };

   // note the size of the buffer is hardcoded
   struct buffer_helper
   {
   protected:
      char m_buffer[ 32 ];

   public:
      static constexpr std::size_t columns = 1;
      static constexpr bool self_contained = true;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return oid::invalid;
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
      void element( std::string& data ) const
      {
         data += m_buffer;
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         data += m_buffer;
      }
   };

   // all uses of this helper need to fit into the buffer, see above
   struct to_chars_helper
      : buffer_helper
   {
      explicit to_chars_helper( const auto v ) noexcept
      {
         const auto [ ptr, ec ] = std::to_chars( std::begin( m_buffer ), std::end( m_buffer ), v );
         assert( ec == std::errc() );
         *ptr = '\0';
      }
   };

}  // namespace tao::pq::internal

#endif
