// Copyright (c) 2021-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_BINARY_HPP
#define TAO_PQ_BINARY_HPP

#include <cstddef>
#include <span>
#include <vector>

namespace tao::pq
{
   using binary = std::vector< std::byte >;
   using binary_view = std::span< const std::byte >;

   [[nodiscard]] auto to_binary_view( const auto* data, const std::size_t size ) noexcept -> binary_view
      requires( sizeof( *data ) == 1 )
   {
      return { reinterpret_cast< const std::byte* >( data ), size };
   }

   [[nodiscard]] auto to_binary_view( const auto& value ) noexcept -> binary_view
   {
      return pq::to_binary_view( std::data( value ), std::size( value ) );
   }

   [[nodiscard]] auto to_binary( const auto* data, const std::size_t size ) -> binary
      requires( sizeof( *data ) == 1 )
   {
      const auto* ptr = reinterpret_cast< const std::byte* >( data );
      return { ptr, ptr + size };
   }

   [[nodiscard]] auto to_binary( const auto& value ) -> binary
   {
      return pq::to_binary( std::data( value ), std::size( value ) );
   }

}  // namespace tao::pq

#endif
