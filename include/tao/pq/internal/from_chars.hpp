// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_FROM_CHARS_HPP
#define TAO_PQ_INTERNAL_FROM_CHARS_HPP

#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>

namespace tao::pq::internal
{
   template< typename T >
   auto from_chars( const std::string_view value ) -> T
   {
      T result;
      const auto [ ptr, ec ] = std::from_chars( value.data(), value.data() + value.size(), result );
      if( ec == std::errc() ) {
         if( ptr == value.data() + value.size() ) {
            return result;
         }
         throw std::runtime_error( std::string( __PRETTY_FUNCTION__ ) + ": invalid argument: " + std::string( value ) );
      }
      switch( ec ) {
         case std::errc::invalid_argument:
            throw std::runtime_error( std::string( __PRETTY_FUNCTION__ ) + ": invalid argument: " + std::string( value ) );
         case std::errc::result_out_of_range:
            throw std::overflow_error( std::string( __PRETTY_FUNCTION__ ) + ": overflow error: " + std::string( value ) );
         default:
            throw std::runtime_error( std::string( __PRETTY_FUNCTION__ ) + ": unknown error: " + std::string( value ) );
      }
   }

}  // namespace tao::pq::internal

#endif
