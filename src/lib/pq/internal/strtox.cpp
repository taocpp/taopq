// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <stdexcept>
#include <string>

#include <tao/pq/internal/printf.hpp>
#include <tao/pq/internal/strtox.hpp>

namespace tao::pq::internal
{
   namespace
   {
      template< typename >
      inline constexpr const char* message = nullptr;

      // clang-format off
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
