// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#ifdef WIN32
int main() {}
#else

#include "../macros.hpp"

#include <cmath>
#include <stdexcept>
#include <typeinfo>

#include <tao/utility/printf.hpp>
#include <tao/utility/strtox.hpp>

template< typename T >
void reject( const char* input, const int base = 10 )
{
   try {
      tao::utility::strtol( input, base );
      throw std::runtime_error( tao::utility::printf( "strtol(): %s (%d)", input, base ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::utility::strtol() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
   try {
      tao::utility::strtoll( input, base );
      throw std::runtime_error( tao::utility::printf( "strtoll(): %s (%d)", input, base ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::utility::strtoll() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
}

template< typename T >
void reject_unsigned( const char* input, const int base = 10 )
{
   try {
      tao::utility::strtoul( input, base );
      throw std::runtime_error( tao::utility::printf( "strtoul(): %s (%d)", input, base ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::utility::strtoul() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
   try {
      tao::utility::strtoull( input, base );
      throw std::runtime_error( tao::utility::printf( "strtoull(): %s (%d)", input, base ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::utility::strtoull() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
}

template< typename T >
void reject_floating_point( const char* input )
{
   try {
      tao::utility::strtof( input );
      throw std::runtime_error( tao::utility::printf( "strtof(): %s", input ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::utility::strtof() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
   try {
      tao::utility::strtod( input );
      throw std::runtime_error( tao::utility::printf( "strtod(): %s", input ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::utility::strtod() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
   try {
      tao::utility::strtold( input );
      throw std::runtime_error( tao::utility::printf( "strtold(): %s", input ) );
   }
   catch( const T& e ) {
      if( e.what() != "tao::utility::strtold() failed for input: " + std::string( input ) ) {
         throw;
      }
   }
}

void run()
{
   TEST_ASSERT( tao::utility::strtol( "0" ) == 0 );
   TEST_ASSERT( tao::utility::strtol( "+0" ) == 0 );
   TEST_ASSERT( tao::utility::strtol( "-0" ) == 0 );
   TEST_ASSERT( tao::utility::strtol( "00" ) == 0 );
   TEST_ASSERT( tao::utility::strtol( "-00" ) == 0 );
   TEST_ASSERT( tao::utility::strtol( "1" ) == 1 );
   TEST_ASSERT( tao::utility::strtol( "+1" ) == 1 );
   TEST_ASSERT( tao::utility::strtol( "-1" ) == -1 );
   TEST_ASSERT( tao::utility::strtol( "01" ) == 1 );
   TEST_ASSERT( tao::utility::strtol( "-01" ) == -1 );
   TEST_ASSERT( tao::utility::strtol( "0000000000000000001" ) == 1 );
   TEST_ASSERT( tao::utility::strtol( "-0000000000000000001" ) == -1 );
   TEST_ASSERT( tao::utility::strtol( "00000000000000000000000000000000000001" ) == 1 );
   TEST_ASSERT( tao::utility::strtol( "-00000000000000000000000000000000000001" ) == -1 );
#ifdef __LP64__
   TEST_ASSERT( tao::utility::strtol( "9223372036854775807" ) == 9223372036854775807 );
   TEST_ASSERT( tao::utility::strtol( "-9223372036854775808" ) == -9223372036854775807 - 1 );
#else
   TEST_ASSERT( tao::utility::strtol( "2147483647" ) == 2147483647 );
   TEST_ASSERT( tao::utility::strtol( "-2147483648" ) == -2147483647 - 1 );
#endif

   TEST_ASSERT( tao::utility::strtol( "0", 2 ) == 0 );
   TEST_ASSERT( tao::utility::strtol( "0", 36 ) == 0 );

   TEST_ASSERT( tao::utility::strtoll( "0" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "+0" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "-0" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "00" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "+00" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "-00" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "1" ) == 1 );
   TEST_ASSERT( tao::utility::strtoll( "+1" ) == 1 );
   TEST_ASSERT( tao::utility::strtoll( "-1" ) == -1 );
   TEST_ASSERT( tao::utility::strtoll( "01" ) == 1 );
   TEST_ASSERT( tao::utility::strtoll( "+01" ) == 1 );
   TEST_ASSERT( tao::utility::strtoll( "-01" ) == -1 );
   TEST_ASSERT( tao::utility::strtoll( "0000000000000000000" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "+0000000000000000000" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "-0000000000000000000" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "0000000000000000001" ) == 1 );
   TEST_ASSERT( tao::utility::strtoll( "+0000000000000000001" ) == 1 );
   TEST_ASSERT( tao::utility::strtoll( "-0000000000000000001" ) == -1 );
   TEST_ASSERT( tao::utility::strtoll( "9223372036854775807" ) == 9223372036854775807 );
   TEST_ASSERT( tao::utility::strtoll( "+9223372036854775807" ) == 9223372036854775807 );
   TEST_ASSERT( tao::utility::strtoll( "-9223372036854775808" ) == -9223372036854775807 - 1 );
   TEST_ASSERT( tao::utility::strtoll( "00000000000000000000000000000000000000" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "+00000000000000000000000000000000000000" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "-00000000000000000000000000000000000000" ) == 0 );
   TEST_ASSERT( tao::utility::strtoll( "00000000000000000000000000000000000001" ) == 1 );
   TEST_ASSERT( tao::utility::strtoll( "+00000000000000000000000000000000000001" ) == 1 );
   TEST_ASSERT( tao::utility::strtoll( "-00000000000000000000000000000000000001" ) == -1 );
   TEST_ASSERT( tao::utility::strtoll( "00000000000000000009223372036854775807", 10 ) == 9223372036854775807 );
   TEST_ASSERT( tao::utility::strtoll( "+00000000000000000009223372036854775807", 10 ) == 9223372036854775807 );
   TEST_ASSERT( tao::utility::strtoll( "-00000000000000000009223372036854775808", 10 ) == -9223372036854775807 - 1 );

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

   TEST_ASSERT( tao::utility::strtoul( "0" ) == 0 );
   TEST_ASSERT( tao::utility::strtoul( "+0" ) == 0 );
   TEST_ASSERT( tao::utility::strtoul( "-0" ) == 0 );
   TEST_ASSERT( tao::utility::strtoul( "00" ) == 0 );
   TEST_ASSERT( tao::utility::strtoul( "-00" ) == 0 );
   TEST_ASSERT( tao::utility::strtoul( "1" ) == 1 );
   TEST_ASSERT( tao::utility::strtoul( "+1" ) == 1 );
   TEST_ASSERT( tao::utility::strtoul( "01" ) == 1 );
   TEST_ASSERT( tao::utility::strtoul( "0000000000000000001" ) == 1 );
   TEST_ASSERT( tao::utility::strtoul( "00000000000000000000000000000000000001" ) == 1 );
#ifdef __LP64__
   TEST_ASSERT( tao::utility::strtoul( "18446744073709551615" ) == 18446744073709551615U );
#else
   TEST_ASSERT( tao::utility::strtoul( "4294967295" ) == 4294967295U );
#endif

   TEST_ASSERT( tao::utility::strtoull( "0" ) == 0 );
   TEST_ASSERT( tao::utility::strtoull( "1" ) == 1 );
   TEST_ASSERT( tao::utility::strtoull( "18446744073709551615" ) == 18446744073709551615U );

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

   TEST_ASSERT( tao::utility::strtof( "0" ) == 0 );
   TEST_ASSERT( tao::utility::strtof( "1" ) == 1 );
   TEST_ASSERT( tao::utility::strtof( "00" ) == 0 );
   TEST_ASSERT( tao::utility::strtof( "01" ) == 1 );
   TEST_ASSERT( tao::utility::strtof( "0." ) == 0 );
   TEST_ASSERT( tao::utility::strtof( "1." ) == 1 );
   TEST_ASSERT( tao::utility::strtof( "0.0" ) == 0 );
   TEST_ASSERT( tao::utility::strtof( "1.0" ) == 1 );
   TEST_ASSERT( tao::utility::strtof( "0.5" ) == .5 );
   TEST_ASSERT( tao::utility::strtof( ".5" ) == .5 );
   TEST_ASSERT( tao::utility::strtof( ".25" ) == .25 );
   TEST_ASSERT( tao::utility::strtof( ".125" ) == .125 );
   TEST_ASSERT( tao::utility::strtof( ".0625" ) == .0625 );
   TEST_ASSERT( tao::utility::strtof( ".4375" ) == .4375 );

   TEST_ASSERT( tao::utility::strtof( "-0" ) == 0 );
   TEST_ASSERT( tao::utility::strtof( "-1" ) == -1 );
   TEST_ASSERT( tao::utility::strtof( "-00" ) == 0 );
   TEST_ASSERT( tao::utility::strtof( "-01" ) == -1 );
   TEST_ASSERT( tao::utility::strtof( "-0." ) == 0 );
   TEST_ASSERT( tao::utility::strtof( "-1." ) == -1 );
   TEST_ASSERT( tao::utility::strtof( "-0.0" ) == 0 );
   TEST_ASSERT( tao::utility::strtof( "-1.0" ) == -1 );
   TEST_ASSERT( tao::utility::strtof( "-0.5" ) == -.5 );
   TEST_ASSERT( tao::utility::strtof( "-.5" ) == -.5 );
   TEST_ASSERT( tao::utility::strtof( "-.25" ) == -.25 );
   TEST_ASSERT( tao::utility::strtof( "-.125" ) == -.125 );
   TEST_ASSERT( tao::utility::strtof( "-.0625" ) == -.0625 );
   TEST_ASSERT( tao::utility::strtof( "-.4375" ) == -.4375 );

   TEST_ASSERT( tao::utility::strtof( "3.1415927410125732421875" ) == 3.1415927410125732421875 );
   TEST_ASSERT( tao::utility::strtod( "3.1415927410125732421875" ) == 3.1415927410125732421875 );
   TEST_ASSERT( tao::utility::strtold( "3.1415927410125732421875" ) == 3.1415927410125732421875 );

   TEST_ASSERT( tao::utility::strtof( "0000000000000000000000000000000000000.0000000000000000000000000000000000000" ) == 0 );
   TEST_ASSERT( tao::utility::strtof( "0000000000000000000000000000000000001.0000000000000000000000000000000000000" ) == 1 );

   TEST_ASSERT( std::isinf( tao::utility::strtof( "inf" ) ) );
   TEST_ASSERT( std::isinf( tao::utility::strtof( "INF" ) ) );
   TEST_ASSERT( std::isinf( tao::utility::strtof( "infinity" ) ) );
   TEST_ASSERT( std::isinf( tao::utility::strtof( "INFINITY" ) ) );
   TEST_ASSERT( std::isinf( tao::utility::strtof( "-inf" ) ) );
   TEST_ASSERT( std::isinf( tao::utility::strtof( "-INF" ) ) );
   TEST_ASSERT( std::isinf( tao::utility::strtof( "-infinity" ) ) );
   TEST_ASSERT( std::isinf( tao::utility::strtof( "-INFINITY" ) ) );
   TEST_ASSERT( std::isnan( tao::utility::strtof( "nan" ) ) );
   TEST_ASSERT( std::isnan( tao::utility::strtof( "NaN" ) ) );
   TEST_ASSERT( std::isnan( tao::utility::strtof( "NAN" ) ) );

   TEST_ASSERT( tao::utility::strtof( "inf" ) > 0 );
   TEST_ASSERT( tao::utility::strtof( "-inf" ) < 0 );

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

int main()
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
