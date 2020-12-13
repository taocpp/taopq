// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <tao/pq/internal/printf.hpp>
#include <tao/pq/internal/strtox.hpp>

namespace tao::pq::internal
{
   namespace
   {
      template< typename >
      inline constexpr const char* message = nullptr;

      // clang-format off
      template<> inline constexpr auto message< long > = "tao::pq::internal::strtol() failed for input: ";
      template<> inline constexpr auto message< unsigned long > = "tao::pq::internal::strtoul() failed for input: ";
      template<> inline constexpr auto message< long long > = "tao::pq::internal::strtoll() failed for input: ";
      template<> inline constexpr auto message< unsigned long long > = "tao::pq::internal::strtoull() failed for input: ";

      template<> inline constexpr auto message< float > = "tao::pq::internal::strtof() failed for input: ";
      template<> inline constexpr auto message< double > = "tao::pq::internal::strtod() failed for input: ";
      template<> inline constexpr auto message< long double > = "tao::pq::internal::strtold() failed for input: ";
      // clang-format on

      template< typename T >
      [[nodiscard]] auto failure_message( const char* input ) -> std::string
      {
         return std::string( message< T > ) + input;
      }

      template< typename T >
      [[nodiscard]] auto call_integral( const char* nptr, char** endptr, int base ) -> T;

      template<>
      [[nodiscard]] auto call_integral< long >( const char* nptr, char** endptr, int base ) -> long
      {
         return std::strtol( nptr, endptr, base );
      }

      template<>
      [[nodiscard]] auto call_integral< unsigned long >( const char* nptr, char** endptr, int base ) -> unsigned long
      {
         return std::strtoul( nptr, endptr, base );
      }

      template<>
      [[nodiscard]] auto call_integral< long long >( const char* nptr, char** endptr, int base ) -> long long
      {
         return std::strtoll( nptr, endptr, base );
      }

      template<>
      [[nodiscard]] auto call_integral< unsigned long long >( const char* nptr, char** endptr, int base ) -> unsigned long long
      {
         return std::strtoull( nptr, endptr, base );
      }

      template< typename T >
      [[nodiscard]] auto call_floating_point( const char* nptr, char** endptr ) -> T;

      template<>
      [[nodiscard]] auto call_floating_point< float >( const char* nptr, char** endptr ) -> float
      {
         return std::strtof( nptr, endptr );
      }

      template<>
      [[nodiscard]] auto call_floating_point< double >( const char* nptr, char** endptr ) -> double
      {
         return std::strtod( nptr, endptr );
      }

      template<>
      [[nodiscard]] auto call_floating_point< long double >( const char* nptr, char** endptr ) -> long double
      {
         return std::strtold( nptr, endptr );
      }

      template< typename T >
      [[nodiscard]] auto str_to_integral( const char* input, const int base ) -> T
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
               [[fallthrough]];

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
               throw std::runtime_error( tao::pq::internal::printf( "code should be unreachable, errno: ERANGE, input: \"%s\"", input ) );  // LCOV_EXCL_LINE
         }
         throw std::runtime_error( tao::pq::internal::printf( "code should be unreachable, errno: %d, input: \"%s\"", errno, input ) );  // LCOV_EXCL_LINE
      }

      template< typename T >
      [[nodiscard]] auto str_to_floating_point( const char* input ) -> T
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
         throw std::runtime_error( tao::pq::internal::printf( "code should be unreachable, errno: %d, input: \"%s\"", errno, input ) );  // LCOV_EXCL_LINE
      }

   }  // namespace

   [[nodiscard]] auto strtol( const char* input, const int base ) -> long
   {
      return str_to_integral< long >( input, base );
   }

   [[nodiscard]] auto strtoul( const char* input, const int base ) -> unsigned long
   {
      return str_to_integral< unsigned long >( input, base );
   }

   [[nodiscard]] auto strtoll( const char* input, const int base ) -> long long
   {
      return str_to_integral< long long >( input, base );
   }

   [[nodiscard]] auto strtoull( const char* input, const int base ) -> unsigned long long
   {
      return str_to_integral< unsigned long long >( input, base );
   }

   [[nodiscard]] auto strtof( const char* input ) -> float
   {
      return str_to_floating_point< float >( input );
   }

   [[nodiscard]] auto strtod( const char* input ) -> double
   {
      return str_to_floating_point< double >( input );
   }

   [[nodiscard]] auto strtold( const char* input ) -> long double
   {
      return str_to_floating_point< long double >( input );
   }

}  // namespace tao::pq::internal
