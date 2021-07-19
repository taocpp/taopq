// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/result_traits.hpp>

#include <limits>
#include <memory>
#include <stdexcept>

#include <libpq-fe.h>

#include <tao/pq/internal/strtox.hpp>

namespace tao::pq
{
   auto result_traits< std::basic_string< std::byte > >::from( const char* value ) -> std::basic_string< std::byte >
   {
      std::size_t size;
      const std::unique_ptr< std::byte, decltype( &PQfreemem ) > buffer( (std::byte*)PQunescapeBytea( (unsigned char*)value, &size ), &PQfreemem );
      if( !buffer ) {
         throw std::bad_alloc();  // LCOV_EXCL_LINE
      }
      return std::basic_string< std::byte >( buffer.get(), size );
   }

   auto result_traits< bool >::from( const char* value ) -> bool
   {
      if( value[ 0 ] != '\0' && value[ 1 ] == '\0' ) {
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
      if( value[ 0 ] == '\0' || value[ 1 ] != '\0' ) {
         throw std::runtime_error( "invalid value in tao::pq::result_traits<char> for input: " + std::string( value ) );
      }
      return value[ 0 ];
   }

   auto result_traits< signed char >::from( const char* value ) -> signed char
   {
      const long v = internal::strtol( value, 10 );
      if( v < std::numeric_limits< signed char >::min() ) {
         throw std::underflow_error( "underflow error in tao::pq::result_traits<signed char> for input: " + std::string( value ) );
      }
      if( v > std::numeric_limits< signed char >::max() ) {
         throw std::overflow_error( "overflow error in tao::pq::result_traits<signed char> for input: " + std::string( value ) );
      }
      return static_cast< signed char >( v );
   }

   auto result_traits< unsigned char >::from( const char* value ) -> unsigned char
   {
      const unsigned long v = internal::strtoul( value, 10 );
      if( v > std::numeric_limits< unsigned char >::max() ) {
         throw std::overflow_error( "overflow error in tao::pq::result_traits<unsigned char> for input: " + std::string( value ) );
      }
      return static_cast< unsigned char >( v );
   }

   auto result_traits< short >::from( const char* value ) -> short
   {
      const long v = internal::strtol( value, 10 );
      if( v < std::numeric_limits< short >::min() ) {
         throw std::underflow_error( "underflow error in tao::pq::result_traits<short> for input: " + std::string( value ) );
      }
      if( v > std::numeric_limits< short >::max() ) {
         throw std::overflow_error( "overflow error in tao::pq::result_traits<short> for input: " + std::string( value ) );
      }
      return static_cast< short >( v );
   }

   auto result_traits< unsigned short >::from( const char* value ) -> unsigned short
   {
      const unsigned long v = internal::strtoul( value, 10 );
      if( v > std::numeric_limits< unsigned short >::max() ) {
         throw std::overflow_error( "overflow error in tao::pq::result_traits<unsigned short> for input: " + std::string( value ) );
      }
      return static_cast< unsigned short >( v );
   }

   auto result_traits< int >::from( const char* value ) -> int
   {
      const long v = internal::strtol( value, 10 );
      if( v < std::numeric_limits< int >::min() ) {
         throw std::underflow_error( "underflow error in tao::pq::result_traits<int> for input: " + std::string( value ) );
      }
      if( v > std::numeric_limits< int >::max() ) {
         throw std::overflow_error( "overflow error in tao::pq::result_traits<int> for input: " + std::string( value ) );
      }
      return static_cast< int >( v );
   }

   auto result_traits< unsigned >::from( const char* value ) -> unsigned
   {
      const unsigned long v = internal::strtoul( value, 10 );
      if( v > std::numeric_limits< unsigned >::max() ) {
         throw std::overflow_error( "overflow error in tao::pq::result_traits<unsigned> for input: " + std::string( value ) );
      }
      return static_cast< unsigned >( v );
   }

   auto result_traits< long >::from( const char* value ) -> long
   {
      return internal::strtol( value, 10 );
   }

   auto result_traits< unsigned long >::from( const char* value ) -> unsigned long
   {
      return internal::strtoul( value, 10 );
   }

   auto result_traits< long long >::from( const char* value ) -> long long
   {
      return internal::strtoll( value, 10 );
   }

   auto result_traits< unsigned long long >::from( const char* value ) -> unsigned long long
   {
      return internal::strtoull( value, 10 );
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

}  // namespace tao::pq
