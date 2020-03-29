// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifdef WIN32
auto main() -> int {}
#else

#include "../macros.hpp"

#include <cmath>
#include <stdexcept>
#include <typeinfo>

#include <tao/pq/internal/printf.hpp>
#include <tao/pq/internal/strtox.hpp>

template< typename T >
void reject( const char* input, const int base = 10 )
{
   try {
      (void)tao::pq::internal::strtol( input, base );
      throw std::runtime_error( tao::pq::internal::printf( "strtol(): %s (%d)", input, base ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::pq::internal::strtol() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
   try {
      (void)tao::pq::internal::strtoll( input, base );
      throw std::runtime_error( tao::pq::internal::printf( "strtoll(): %s (%d)", input, base ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::pq::internal::strtoll() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
}

template< typename T >
void reject_unsigned( const char* input, const int base = 10 )
{
   try {
      (void)tao::pq::internal::strtoul( input, base );
      throw std::runtime_error( tao::pq::internal::printf( "strtoul(): %s (%d)", input, base ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::pq::internal::strtoul() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
   try {
      (void)tao::pq::internal::strtoull( input, base );
      throw std::runtime_error( tao::pq::internal::printf( "strtoull(): %s (%d)", input, base ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::pq::internal::strtoull() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
}

template< typename T >
void reject_floating_point( const char* input )
{
   try {
      (void)tao::pq::internal::strtof( input );
      throw std::runtime_error( tao::pq::internal::printf( "strtof(): %s", input ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::pq::internal::strtof() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
   try {
      (void)tao::pq::internal::strtod( input );
      throw std::runtime_error( tao::pq::internal::printf( "strtod(): %s", input ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::pq::internal::strtod() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
   try {
      (void)tao::pq::internal::strtold( input );
      throw std::runtime_error( tao::pq::internal::printf( "strtold(): %s", input ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::pq::internal::strtold() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
}

void run()  // NOLINT(readability-function-size)
{
   TEST_ASSERT( tao::pq::internal::strtol( "0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtol( "+0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtol( "-0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtol( "00" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtol( "-00" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtol( "1" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtol( "+1" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtol( "-1" ) == -1 );
   TEST_ASSERT( tao::pq::internal::strtol( "01" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtol( "-01" ) == -1 );
   TEST_ASSERT( tao::pq::internal::strtol( "0000000000000000001" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtol( "-0000000000000000001" ) == -1 );
   TEST_ASSERT( tao::pq::internal::strtol( "00000000000000000000000000000000000001" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtol( "-00000000000000000000000000000000000001" ) == -1 );
#ifdef __LP64__
   TEST_ASSERT( tao::pq::internal::strtol( "9223372036854775807" ) == 9223372036854775807 );
   TEST_ASSERT( tao::pq::internal::strtol( "-9223372036854775808" ) == -9223372036854775807 - 1 );
#else
   TEST_ASSERT( tao::pq::internal::strtol( "2147483647" ) == 2147483647 );
   TEST_ASSERT( tao::pq::internal::strtol( "-2147483648" ) == -2147483647 - 1 );
#endif

   TEST_ASSERT( tao::pq::internal::strtol( "0", 2 ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtol( "0", 36 ) == 0 );

   TEST_ASSERT( tao::pq::internal::strtoll( "0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "00" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+00" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-00" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "1" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+1" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-1" ) == -1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "01" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+01" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-01" ) == -1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "0000000000000000000" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+0000000000000000000" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-0000000000000000000" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "0000000000000000001" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+0000000000000000001" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-0000000000000000001" ) == -1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "9223372036854775807" ) == 9223372036854775807 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+9223372036854775807" ) == 9223372036854775807 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-9223372036854775808" ) == -9223372036854775807 - 1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "00000000000000000000000000000000000000" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+00000000000000000000000000000000000000" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-00000000000000000000000000000000000000" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoll( "00000000000000000000000000000000000001" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+00000000000000000000000000000000000001" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-00000000000000000000000000000000000001" ) == -1 );
   TEST_ASSERT( tao::pq::internal::strtoll( "00000000000000000009223372036854775807", 10 ) == 9223372036854775807 );
   TEST_ASSERT( tao::pq::internal::strtoll( "+00000000000000000009223372036854775807", 10 ) == 9223372036854775807 );
   TEST_ASSERT( tao::pq::internal::strtoll( "-00000000000000000009223372036854775808", 10 ) == -9223372036854775807 - 1 );

   reject< std::runtime_error >( "" );
   reject< std::runtime_error >( " " );
   reject< std::runtime_error >( " 0" );
   reject< std::runtime_error >( " 1" );
   reject< std::runtime_error >( " +0" );
   reject< std::runtime_error >( " +1" );
   reject< std::runtime_error >( " -0" );
   reject< std::runtime_error >( " -1" );
   reject< std::runtime_error >( "+" );
   reject< std::runtime_error >( "++" );
   reject< std::runtime_error >( "+-" );
   reject< std::runtime_error >( "-" );
   reject< std::runtime_error >( "--" );
   reject< std::runtime_error >( "-+" );
   reject< std::runtime_error >( "++0" );
   reject< std::runtime_error >( "+-0" );
   reject< std::runtime_error >( "--0" );
   reject< std::runtime_error >( "-+0" );
   reject< std::runtime_error >( "+0+" );
   reject< std::runtime_error >( "+0-" );
   reject< std::runtime_error >( "-0-" );
   reject< std::runtime_error >( "-0+" );
   reject< std::runtime_error >( "0+" );
   reject< std::runtime_error >( "0-" );
   reject< std::runtime_error >( "0 " );
   reject< std::runtime_error >( "0x" );
   reject< std::runtime_error >( "1 " );

   // TODO: Should this yield a different error message? Or even be a std::logic_error?
   reject< std::runtime_error >( "0", 1 );
   reject< std::runtime_error >( "0", 37 );

   reject< std::runtime_error >( "0xABCDEFG", 16 );
   reject< std::overflow_error >( "0x8000000000000000", 16 );
   reject< std::overflow_error >( "0xFFFFFFFFFFFFFFFF", 16 );
   reject< std::overflow_error >( "0x10000000000000000", 16 );

   reject< std::overflow_error >( "9223372036854775808" );
   reject< std::overflow_error >( "+9223372036854775808" );
   reject< std::underflow_error >( "-9223372036854775809" );

   reject< std::overflow_error >( "9999999999999999999" );
   reject< std::overflow_error >( "+9999999999999999999" );
   reject< std::underflow_error >( "-9999999999999999999" );

   reject< std::overflow_error >( "99999999999999999999999999999999999999" );
   reject< std::overflow_error >( "+99999999999999999999999999999999999999" );
   reject< std::underflow_error >( "-99999999999999999999999999999999999999" );

   TEST_ASSERT( tao::pq::internal::strtoul( "0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoul( "+0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoul( "-0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoul( "00" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoul( "-00" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoul( "1" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoul( "+1" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoul( "01" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoul( "0000000000000000001" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoul( "00000000000000000000000000000000000001" ) == 1 );
#ifdef __LP64__
   TEST_ASSERT( tao::pq::internal::strtoul( "18446744073709551615" ) == 18446744073709551615U );
#else
   TEST_ASSERT( tao::pq::internal::strtoul( "4294967295" ) == 4294967295U );
#endif

   TEST_ASSERT( tao::pq::internal::strtoull( "0" ) == 0 );
   TEST_ASSERT( tao::pq::internal::strtoull( "1" ) == 1 );
   TEST_ASSERT( tao::pq::internal::strtoull( "18446744073709551615" ) == 18446744073709551615U );

   reject_unsigned< std::runtime_error >( "" );
   reject_unsigned< std::runtime_error >( " " );
   reject_unsigned< std::runtime_error >( "+" );
   reject_unsigned< std::runtime_error >( "-" );
   reject_unsigned< std::runtime_error >( " 0" );
   reject_unsigned< std::runtime_error >( "0 " );
   reject_unsigned< std::runtime_error >( "0x" );
   reject_unsigned< std::runtime_error >( " 1" );
   reject_unsigned< std::runtime_error >( "1 " );

   reject_unsigned< std::overflow_error >( "18446744073709551616" );
   reject_unsigned< std::overflow_error >( "99999999999999999999" );
   reject_unsigned< std::overflow_error >( "99999999999999999999999999999999999999" );

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

auto main() -> int  // NOLINT(bugprone-exception-escape)
{
   try {
      run();
   }
   catch( const std::exception& e ) {
      std::cerr << "exception: " << e.what() << std::endl;
      throw;
   }
   catch( ... ) {
      std::cerr << "unknown exception" << std::endl;
      throw;
   }
}

#endif
