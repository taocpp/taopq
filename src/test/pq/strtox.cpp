// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#if defined( _WIN32 )
auto main() -> int {}
#else

#include "../macros.hpp"

#include <cmath>
#include <exception>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>

#include <tao/pq/internal/strtox.hpp>

namespace
{
   template< typename T >
   void reject_floating_point( const char* input )
   {
      try {
         std::ignore = tao::pq::internal::strtof( input );
         throw std::runtime_error( std::format( "strtof(): {}", input ) );  // LCOV_EXCL_LINE
      }
      catch( const T& e ) {
         if( e.what() != "tao::pq::internal::strtof() failed for input: " + std::string( input ) ) {
            throw;  // LCOV_EXCL_LINE
         }
      }
      try {
         std::ignore = tao::pq::internal::strtod( input );
         throw std::runtime_error( std::format( "strtod(): {}", input ) );  // LCOV_EXCL_LINE
      }
      catch( const T& e ) {
         if( e.what() != "tao::pq::internal::strtod() failed for input: " + std::string( input ) ) {
            throw;  // LCOV_EXCL_LINE
         }
      }
      try {
         std::ignore = tao::pq::internal::strtold( input );
         throw std::runtime_error( std::format( "strtold(): {}", input ) );  // LCOV_EXCL_LINE
      }
      catch( const T& e ) {
         if( e.what() != "tao::pq::internal::strtold() failed for input: " + std::string( input ) ) {
            throw;  // LCOV_EXCL_LINE
         }
      }
   }

   void run()
   {
      TEST_ASSERT( tao::pq::internal::strtof( "0" ) == 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "1" ) == 1 );
      TEST_ASSERT( tao::pq::internal::strtof( "00" ) == 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "01" ) == 1 );
      TEST_ASSERT( tao::pq::internal::strtof( "0." ) == 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "1." ) == 1 );
      TEST_ASSERT( tao::pq::internal::strtof( "0.0" ) == 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "1.0" ) == 1 );
      TEST_ASSERT( tao::pq::internal::strtof( "0.5" ) == .5 );
      TEST_ASSERT( tao::pq::internal::strtof( ".5" ) == .5 );
      TEST_ASSERT( tao::pq::internal::strtof( ".25" ) == .25 );
      TEST_ASSERT( tao::pq::internal::strtof( ".125" ) == .125 );
      TEST_ASSERT( tao::pq::internal::strtof( ".0625" ) == .0625 );
      TEST_ASSERT( tao::pq::internal::strtof( ".4375" ) == .4375 );

      TEST_ASSERT( tao::pq::internal::strtof( "-0" ) == 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "-1" ) == -1 );
      TEST_ASSERT( tao::pq::internal::strtof( "-00" ) == 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "-01" ) == -1 );
      TEST_ASSERT( tao::pq::internal::strtof( "-0." ) == 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "-1." ) == -1 );
      TEST_ASSERT( tao::pq::internal::strtof( "-0.0" ) == 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "-1.0" ) == -1 );
      TEST_ASSERT( tao::pq::internal::strtof( "-0.5" ) == -.5 );
      TEST_ASSERT( tao::pq::internal::strtof( "-.5" ) == -.5 );
      TEST_ASSERT( tao::pq::internal::strtof( "-.25" ) == -.25 );
      TEST_ASSERT( tao::pq::internal::strtof( "-.125" ) == -.125 );
      TEST_ASSERT( tao::pq::internal::strtof( "-.0625" ) == -.0625 );
      TEST_ASSERT( tao::pq::internal::strtof( "-.4375" ) == -.4375 );

      TEST_ASSERT( tao::pq::internal::strtof( "3.1415927410125732421875" ) == 3.1415927410125732421875 );
      TEST_ASSERT( tao::pq::internal::strtod( "3.1415927410125732421875" ) == 3.1415927410125732421875 );
      TEST_ASSERT( tao::pq::internal::strtold( "3.1415927410125732421875" ) == 3.1415927410125732421875 );

      TEST_ASSERT( tao::pq::internal::strtof( "0000000000000000000000000000000000000.0000000000000000000000000000000000000" ) == 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "0000000000000000000000000000000000001.0000000000000000000000000000000000000" ) == 1 );

      TEST_ASSERT( std::isinf( tao::pq::internal::strtof( "inf" ) ) );
      TEST_ASSERT( std::isinf( tao::pq::internal::strtof( "INF" ) ) );
      TEST_ASSERT( std::isinf( tao::pq::internal::strtof( "infinity" ) ) );
      TEST_ASSERT( std::isinf( tao::pq::internal::strtof( "INFINITY" ) ) );
      TEST_ASSERT( std::isinf( tao::pq::internal::strtof( "-inf" ) ) );
      TEST_ASSERT( std::isinf( tao::pq::internal::strtof( "-INF" ) ) );
      TEST_ASSERT( std::isinf( tao::pq::internal::strtof( "-infinity" ) ) );
      TEST_ASSERT( std::isinf( tao::pq::internal::strtof( "-INFINITY" ) ) );
      TEST_ASSERT( std::isnan( tao::pq::internal::strtof( "nan" ) ) );
      TEST_ASSERT( std::isnan( tao::pq::internal::strtof( "NaN" ) ) );
      TEST_ASSERT( std::isnan( tao::pq::internal::strtof( "NAN" ) ) );

      TEST_ASSERT( tao::pq::internal::strtof( "inf" ) > 0 );
      TEST_ASSERT( tao::pq::internal::strtof( "-inf" ) < 0 );

      reject_floating_point< std::runtime_error >( "" );
      reject_floating_point< std::runtime_error >( " " );
      reject_floating_point< std::runtime_error >( "+" );
      reject_floating_point< std::runtime_error >( "-" );
      reject_floating_point< std::runtime_error >( " 0" );
      reject_floating_point< std::runtime_error >( "0 " );
      reject_floating_point< std::runtime_error >( "0x" );
      reject_floating_point< std::runtime_error >( " 1" );
      reject_floating_point< std::runtime_error >( "1 " );

      reject_floating_point< std::overflow_error >( "1e10000" );
      reject_floating_point< std::overflow_error >( "-1e10000" );
      reject_floating_point< std::underflow_error >( "1e-10000" );
      reject_floating_point< std::underflow_error >( "-1e-10000" );
   }

}  // namespace

auto main() -> int  // NOLINT(bugprone-exception-escape)
{
   try {
      run();
   }
   // LCOV_EXCL_START
   catch( const std::exception& e ) {
      std::cerr << "exception: " << e.what() << '\n';
      throw;
   }
   catch( ... ) {
      std::cerr << "unknown exception\n";
      throw;
   }
   // LCOV_EXCL_STOP
}

#endif
