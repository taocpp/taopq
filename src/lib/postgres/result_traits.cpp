// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#include <tao/postgres/result_traits.hpp>

#include <limits>
#include <stdexcept>

#include <tao/utility/strtox.hpp>

namespace tao
{
   namespace postgres
   {
      bool result_traits< bool >::from( const char* value )
      {
         if( value[ 0 ] != '\0' && value[ 1 ] == '\0' ) {
            if( value[ 0 ] == 't' ) {
               return true;
            }
            if( value[ 0 ] == 'f' ) {
               return false;
            }
         }
         throw std::runtime_error( "invalid value in tao::postgres::result_traits<bool> for input: " + std::string( value ) );
      }

      char result_traits< char >::from( const char* value )
      {
         if( value[ 0 ] == '\0' || value[ 1 ] != '\0' ) {
            throw std::runtime_error( "invalid value in tao::postgres::result_traits<char> for input: " + std::string( value ) );
         }
         return value[ 0 ];
      }

      signed char result_traits< signed char >::from( const char* value )
      {
         const long v = utility::strtol( value, 10 );
         if( v < std::numeric_limits< signed char >::min() ) {
            throw std::underflow_error( "underflow error in tao::postgres::result_traits<signed char> for input: " + std::string( value ) );
         }
         if( v > std::numeric_limits< signed char >::max() ) {
            throw std::overflow_error( "overflow error in tao::postgres::result_traits<signed char> for input: " + std::string( value ) );
         }
         return static_cast< signed char >( v );
      }

      unsigned char result_traits< unsigned char >::from( const char* value )
      {
         const unsigned long v = utility::strtoul( value, 10 );
         if( v > std::numeric_limits< unsigned char >::max() ) {
            throw std::overflow_error( "overflow error in tao::postgres::result_traits<unsigned char> for input: " + std::string( value ) );
         }
         return static_cast< unsigned char >( v );
      }

      short result_traits< short >::from( const char* value )
      {
         const long v = utility::strtol( value, 10 );
         if( v < std::numeric_limits< short >::min() ) {
            throw std::underflow_error( "underflow error in tao::postgres::result_traits<short> for input: " + std::string( value ) );
         }
         if( v > std::numeric_limits< short >::max() ) {
            throw std::overflow_error( "overflow error in tao::postgres::result_traits<short> for input: " + std::string( value ) );
         }
         return static_cast< short >( v );
      }

      unsigned short result_traits< unsigned short >::from( const char* value )
      {
         const unsigned long v = utility::strtoul( value, 10 );
         if( v > std::numeric_limits< unsigned short >::max() ) {
            throw std::overflow_error( "overflow error in tao::postgres::result_traits<unsigned short> for input: " + std::string( value ) );
         }
         return static_cast< unsigned short >( v );
      }

      int result_traits< int >::from( const char* value )
      {
         const long v = utility::strtol( value, 10 );
         if( v < std::numeric_limits< int >::min() ) {
            throw std::underflow_error( "underflow error in tao::postgres::result_traits<int> for input: " + std::string( value ) );
         }
         if( v > std::numeric_limits< int >::max() ) {
            throw std::overflow_error( "overflow error in tao::postgres::result_traits<int> for input: " + std::string( value ) );
         }
         return static_cast< int >( v );
      }

      unsigned result_traits< unsigned >::from( const char* value )
      {
         const unsigned long v = utility::strtoul( value, 10 );
         if( v > std::numeric_limits< unsigned >::max() ) {
            throw std::overflow_error( "overflow error in tao::postgres::result_traits<unsigned> for input: " + std::string( value ) );
         }
         return static_cast< unsigned >( v );
      }

      long result_traits< long >::from( const char* value )
      {
         return utility::strtol( value, 10 );
      }

      unsigned long result_traits< unsigned long >::from( const char* value )
      {
         return utility::strtoul( value, 10 );
      }

      long long result_traits< long long >::from( const char* value )
      {
         return utility::strtoll( value, 10 );
      }

      unsigned long long result_traits< unsigned long long >::from( const char* value )
      {
         return utility::strtoull( value, 10 );
      }

      float result_traits< float >::from( const char* value )
      {
         return utility::strtof( value );
      }

      double result_traits< double >::from( const char* value )
      {
         return utility::strtod( value );
      }

      long double result_traits< long double >::from( const char* value )
      {
         return utility::strtold( value );
      }

   }  // namespace postgres

}  // namespace tao
