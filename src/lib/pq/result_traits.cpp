// Copyright (c) 2016-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/result_traits.hpp>

#include <cstring>
#include <stdexcept>
#include <string>

#include <tao/pq/internal/from_chars.hpp>
#include <tao/pq/internal/resize_uninitialized.hpp>
#include <tao/pq/internal/strtox.hpp>

namespace tao::pq
{
   namespace
   {
      [[nodiscard]] auto unhex( const char c ) -> int
      {
         if( ( c >= '0' ) && ( c <= '9' ) ) {
            return c - '0';
         }
         if( ( c >= 'a' ) && ( c <= 'f' ) ) {
            return c - 'a' + 10;
         }
         throw std::invalid_argument( "unhex failed" );
      }

      template< typename T >
      [[nodiscard]] auto unescape_bytea( const char* value ) -> T
      {
         if( ( value[ 0 ] != '\\' ) || ( value[ 1 ] != 'x' ) ) {
            throw std::invalid_argument( "unescape BYTEA failed: " + std::string( value ) );
         }

         const auto input = std::strlen( value );
         if( input % 2 == 1 ) {
            throw std::invalid_argument( "unescape BYTEA failed: " + std::string( value ) );
         }

         T nrv;
         const auto size = input / 2 - 1;
         internal::resize_uninitialized( nrv, size );
         for( std::size_t pos = 0; pos < size; ++pos ) {
            const auto high = unhex( value[ 2 + 2 * pos ] );
            const auto low = unhex( value[ 2 + 2 * pos + 1 ] );
            nrv[ pos ] = static_cast< typename T::value_type >( ( high << 4 ) | low );
         }
         return nrv;
      }

   }  // namespace

   auto result_traits< bool >::from( const char* value ) -> bool
   {
      if( ( value[ 0 ] != '\0' ) && ( value[ 1 ] == '\0' ) ) {
         if( value[ 0 ] == 't' ) {
            return true;
         }
         if( value[ 0 ] == 'f' ) {
            return false;
         }
      }
      throw std::runtime_error( "invalid value in tao::pq::result_traits<bool> for input: " + std::string( value ) );
   }

   auto result_traits< char >::from( const char* value ) -> char
   {
      if( ( value[ 0 ] == '\0' ) || ( value[ 1 ] != '\0' ) ) {
         throw std::runtime_error( "invalid value in tao::pq::result_traits<char> for input: " + std::string( value ) );
      }
      return value[ 0 ];
   }

   auto result_traits< signed char >::from( const char* value ) -> signed char
   {
      return internal::from_chars< signed char >( value );
   }

   auto result_traits< unsigned char >::from( const char* value ) -> unsigned char
   {
      return internal::from_chars< unsigned char >( value );
   }

   auto result_traits< short >::from( const char* value ) -> short
   {
      return internal::from_chars< short >( value );
   }

   auto result_traits< unsigned short >::from( const char* value ) -> unsigned short
   {
      return internal::from_chars< unsigned short >( value );
   }

   auto result_traits< int >::from( const char* value ) -> int
   {
      return internal::from_chars< int >( value );
   }

   auto result_traits< unsigned >::from( const char* value ) -> unsigned
   {
      return internal::from_chars< unsigned >( value );
   }

   auto result_traits< long >::from( const char* value ) -> long
   {
      return internal::from_chars< long >( value );
   }

   auto result_traits< unsigned long >::from( const char* value ) -> unsigned long
   {
      return internal::from_chars< unsigned long >( value );
   }

   auto result_traits< long long >::from( const char* value ) -> long long
   {
      return internal::from_chars< long long >( value );
   }

   auto result_traits< unsigned long long >::from( const char* value ) -> unsigned long long
   {
      return internal::from_chars< unsigned long long >( value );
   }

   auto result_traits< float >::from( const char* value ) -> float
   {
      return internal::strtof( value );
   }

   auto result_traits< double >::from( const char* value ) -> double
   {
      return internal::strtod( value );
   }

   auto result_traits< long double >::from( const char* value ) -> long double
   {
      return internal::strtold( value );
   }

   auto result_traits< std::basic_string< unsigned char > >::from( const char* value ) -> std::basic_string< unsigned char >
   {
      return unescape_bytea< std::basic_string< unsigned char > >( value );
   }

   auto result_traits< binary >::from( const char* value ) -> binary
   {
      return unescape_bytea< binary >( value );
   }

}  // namespace tao::pq
