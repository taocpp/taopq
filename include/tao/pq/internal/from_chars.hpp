// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_FROM_CHARS_HPP
#define TAO_PQ_INTERNAL_FROM_CHARS_HPP

#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>

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
         throw std::invalid_argument( "tao::pq::internal::from_chars<" + demangle< T >() + ">(): " + std::string( value ) );
      }
      switch( ec ) {
         case std::errc::invalid_argument:
            throw std::invalid_argument( "tao::pq::internal::from_chars<" + demangle< T >() + ">(): " + std::string( value ) );
         case std::errc::result_out_of_range:
            throw std::out_of_range( "tao::pq::internal::from_chars<" + demangle< T >() + ">(): " + std::string( value ) );
         default:                // LCOV_EXCL_LINE
            TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
      }
   }

}  // namespace tao::pq::internal

#endif
