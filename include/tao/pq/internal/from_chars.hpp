// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_FROM_CHARS_HPP
#define TAO_PQ_INTERNAL_FROM_CHARS_HPP

#include <charconv>
#include <format>
#include <stdexcept>
#include <string_view>
#include <system_error>

#include <tao/pq/internal/demangle.hpp>
#include <tao/pq/internal/unreachable.hpp>

namespace tao::pq::internal
{
   template< typename T >
   [[nodiscard]] auto from_chars( const std::string_view value ) -> T
   {
      T result;
      const auto [ ptr, ec ] = std::from_chars( value.data(), value.data() + value.size(), result );
      if( ec == std::errc() ) {
         if( ptr == value.data() + value.size() ) {
            return result;
         }
      }
      const auto type = internal::demangle< T >();
      if( ( ec == std::errc() ) || ( ec == std::errc::invalid_argument ) ) {
         throw std::invalid_argument( std::format( "tao::pq::internal::from_chars<{}>(): {}", type, value ) );
      }
      if( ec == std::errc::result_out_of_range ) {
         throw std::out_of_range( std::format( "tao::pq::internal::from_chars<{}>(): {}", type, value ) );
      }
      TAO_PQ_INTERNAL_UNREACHABLE;  // LCOV_EXCL_LINE
   }

}  // namespace tao::pq::internal

#endif
