// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#include <tao/utility/printf.hpp>
#include <tao/utility/strtox.hpp>

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace tao
{
   namespace utility
   {
      namespace
      {
         template< typename >
         struct message
         {
            static const std::string value;
         };

         // clang-format off
         template<> const std::string message< long >::value = "tao::utility::strtol() failed for input: ";
         template<> const std::string message< unsigned long >::value = "tao::utility::strtoul() failed for input: ";
         template<> const std::string message< long long >::value = "tao::utility::strtoll() failed for input: ";
         template<> const std::string message< unsigned long long >::value = "tao::utility::strtoull() failed for input: ";

         template<> const std::string message< float >::value = "tao::utility::strtof() failed for input: ";
         template<> const std::string message< double >::value = "tao::utility::strtod() failed for input: ";
         template<> const std::string message< long double >::value = "tao::utility::strtold() failed for input: ";
         // clang-format on

         template< typename T >
         std::string failure_message( const char* input )
         {
            return message< T >::value + input;
         }

         template< typename T >
         T call_integral( const char* nptr, char** endptr, int base );

         template<>
         long call_integral< long >( const char* nptr, char** endptr, int base )
         {
            return std::strtol( nptr, endptr, base );
         }

         template<>
         unsigned long call_integral< unsigned long >( const char* nptr, char** endptr, int base )
         {
            return std::strtoul( nptr, endptr, base );
         }

         template<>
         long long call_integral< long long >( const char* nptr, char** endptr, int base )
         {
            return std::strtoll( nptr, endptr, base );
         }

         template<>
         unsigned long long call_integral< unsigned long long >( const char* nptr, char** endptr, int base )
         {
            return std::strtoull( nptr, endptr, base );
         }

         template< typename T >
         T call_floating_point( const char* nptr, char** endptr );

         template<>
         float call_floating_point< float >( const char* nptr, char** endptr )
         {
            return std::strtof( nptr, endptr );
         }

         template<>
         double call_floating_point< double >( const char* nptr, char** endptr )
         {
            return std::strtod( nptr, endptr );
         }

         template<>
         long double call_floating_point< long double >( const char* nptr, char** endptr )
         {
            return std::strtold( nptr, endptr );
         }

         template< typename T >
         T str_to_integral( const char* input, const int base )
         {
            assert( input );
            if( *input == '\0' || std::isspace( *input ) ) {
               throw std::runtime_error( failure_message< T >( input ) );
            }
            char* end;
            errno = 0;
            const T result = call_integral< T >( input, &end, base );
            switch( errno ) {
               case 0:
                  if( *end == '\0' ) {
                     return result;
                  }
                  // fall through

               case EINVAL:
               case EDOM:  // used by MinGW
                  throw std::runtime_error( failure_message< T >( input ) );

               case ERANGE:
                  if( result == std::numeric_limits< T >::max() ) {
                     throw std::overflow_error( failure_message< T >( input ) );
                  }
                  if( std::is_signed< T >::value && result == std::numeric_limits< T >::min() ) {
                     throw std::underflow_error( failure_message< T >( input ) );
                  }
                  throw std::runtime_error( tao::utility::printf( "code should be unreachable, errno: ERANGE, input: \"%s\"", input ) );  // LCOV_EXCL_LINE
            }
            throw std::runtime_error( tao::utility::printf( "code should be unreachable, errno: %d, input: \"%s\"", errno, input ) );  // LCOV_EXCL_LINE
         }

         template< typename T >
         T str_to_floating_point( const char* input )
         {
            assert( input );
            if( *input == '\0' || std::isspace( *input ) ) {
               throw std::runtime_error( failure_message< T >( input ) );
            }
            char* end;
            errno = 0;
            const T result = call_floating_point< T >( input, &end );
            switch( errno ) {
               case 0:
                  if( *end == '\0' ) {
                     return result;
                  }
                  throw std::runtime_error( failure_message< T >( input ) );

               case ERANGE:
                  if( result == 0 ) {
                     throw std::underflow_error( failure_message< T >( input ) );
                  }
                  else {
                     throw std::overflow_error( failure_message< T >( input ) );
                  }
            }
            throw std::runtime_error( tao::utility::printf( "code should be unreachable, errno: %d, input: \"%s\"", errno, input ) );  // LCOV_EXCL_LINE
         }

      }  // namespace

      long strtol( const char* input, const int base )
      {
         return str_to_integral< long >( input, base );
      }

      unsigned long strtoul( const char* input, const int base )
      {
         return str_to_integral< unsigned long >( input, base );
      }

      long long strtoll( const char* input, const int base )
      {
         return str_to_integral< long long >( input, base );
      }

      unsigned long long strtoull( const char* input, const int base )
      {
         return str_to_integral< unsigned long long >( input, base );
      }

      float strtof( const char* input )
      {
         return str_to_floating_point< float >( input );
      }

      double strtod( const char* input )
      {
         return str_to_floating_point< double >( input );
      }

      long double strtold( const char* input )
      {
         return str_to_floating_point< long double >( input );
      }

   }  // namespace utility

}  // namespace tao
