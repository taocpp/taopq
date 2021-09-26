// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_BINARY_HPP
#define TAO_PQ_BINARY_HPP

#include <cstddef>
#include <string>
#include <string_view>

namespace tao::pq
{
   using binary = std::basic_string< std::byte >;
   using binary_view = std::basic_string_view< std::byte >;

   template< typename T >
   [[nodiscard]] auto to_binary_view( const T* data, const std::size_t size ) noexcept -> binary_view
   {
      static_assert( sizeof( T ) == 1 );
      return { reinterpret_cast< const std::byte* >( data ), size };
   }

   template< typename T >
   [[nodiscard]] auto to_binary_view( const T& data ) noexcept -> binary_view
   {
      return pq::to_binary_view( std::data( data ), std::size( data ) );
   }

   template< typename T >
   [[nodiscard]] auto to_binary( const T* data, const std::size_t size ) -> binary
   {
      static_assert( sizeof( T ) == 1 );
      return { reinterpret_cast< const std::byte* >( data ), size };
   }

   template< typename T >
   [[nodiscard]] auto to_binary( const T& data ) -> binary
   {
      return pq::to_binary( std::data( data ), std::size( data ) );
   }

}  // namespace tao::pq

#endif
