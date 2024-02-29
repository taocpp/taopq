// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/binary.hpp>
#include <tao/pq/connection.hpp>

std::shared_ptr< tao::pq::connection > my_connection;

auto prepare_datatype( const std::string& datatype ) -> bool
{
   static std::string last;
   if( datatype == last ) {
      return false;
   }
   last = datatype;
   my_connection->execute( "DROP TABLE IF EXISTS tao_basic_datatypes_test" );
   my_connection->execute( "CREATE TABLE tao_basic_datatypes_test ( a " + datatype + " )" );
   return true;
}

void check_null( const std::string& datatype )
{
   std::cout << "check null: " << datatype << std::endl;
   if( prepare_datatype( datatype ) ) {
      TEST_ASSERT( my_connection->execute( "INSERT INTO tao_basic_datatypes_test VALUES ( $1 )", tao::pq::null ).rows_affected() == 1 );
   }
   else {
      TEST_ASSERT( my_connection->execute( "UPDATE tao_basic_datatypes_test SET a=$1", tao::pq::null ).rows_affected() == 1 );
   }
   const auto result = my_connection->execute( "SELECT * FROM tao_basic_datatypes_test" );
   TEST_ASSERT( result[ 0 ][ 0 ].is_null() );
}

template< typename T >
void check( const std::string& datatype, const T& value )
{
   std::cout << "check: " << datatype << " value: " << value << std::endl;
   TEST_ASSERT( !prepare_datatype( datatype ) );
   TEST_ASSERT( my_connection->execute( "UPDATE tao_basic_datatypes_test SET a=$1", value ).rows_affected() == 1 );

   const auto result = my_connection->execute( "SELECT * FROM tao_basic_datatypes_test" );
   if( value == value ) {  // NOLINT(misc-redundant-expression)
      if( result[ 0 ][ 0 ].as< T >() != value ) {
         // LCOV_EXCL_START
         std::cout << "check: " << datatype << " value: " << value << " result: " << result.get( 0, 0 ) << " FAILED!" << std::endl;
         TEST_ASSERT( false );
         // LCOV_EXCL_STOP
      }
   }
   else {
      const auto v = result[ 0 ][ 0 ].as< T >();
      if( v == v ) {  // NOLINT(misc-redundant-expression)
         // LCOV_EXCL_START
         std::cout << "check: " << datatype << " value: NaN result: " << result.get( 0, 0 ) << " FAILED!" << std::endl;
         TEST_ASSERT( false );
         // LCOV_EXCL_STOP
      }
   }
}

template< typename T >
auto check( const std::string& datatype )
   -> std::enable_if_t< std::is_signed_v< T > >
{
   check_null( datatype );
   check< T >( datatype, std::numeric_limits< T >::min() );
   check_null( datatype );
   check< T >( datatype, -42 );
   check< T >( datatype, -1 );
   check< T >( datatype, 0 );
   check< T >( datatype, 1 );
   check< T >( datatype, 42 );
   check< T >( datatype, std::numeric_limits< T >::max() );
}

template< typename T >
auto check( const std::string& datatype )
   -> std::enable_if_t< std::is_unsigned_v< T > >
{
   check_null( datatype );
   check< T >( datatype, 0 );
   check_null( datatype );
   check< T >( datatype, 1 );
   check< T >( datatype, 42 );
   check< T >( datatype, std::numeric_limits< T >::max() );
}

template< typename T >
void check_bytea( T&& t )
{
   TEST_ASSERT( my_connection->execute( "UPDATE tao_basic_datatypes_test SET a=$1", std::forward< T >( t ) ).rows_affected() == 1 );

   const auto result = my_connection->execute( "SELECT * FROM tao_basic_datatypes_test" )[ 0 ][ 0 ].as< std::basic_string< typename T::value_type > >();
   TEST_ASSERT( result.size() == 7 );
   TEST_ASSERT( result[ 0 ] == t[ 0 ] );
   TEST_ASSERT( result[ 1 ] == t[ 1 ] );
   TEST_ASSERT( result[ 2 ] == t[ 2 ] );
   TEST_ASSERT( result[ 3 ] == t[ 3 ] );
   TEST_ASSERT( result[ 4 ] == t[ 4 ] );
   TEST_ASSERT( result[ 5 ] == t[ 5 ] );
   TEST_ASSERT( result[ 6 ] == t[ 6 ] );
}

void run()
{
   my_connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );
   my_connection->set_timeout( std::chrono::seconds( 1 ) );

   check_null( "BOOLEAN" );
   check< bool >( "BOOLEAN", true );
   check_null( "BOOLEAN" );
   check< bool >( "BOOLEAN", false );

   // single characters in PostgreSQL are stored in a column of type "char", note the quotes are part of the type's name!
   check_null( "\"char\"" );
   check< char >( "\"char\"", 'a' );
   check_null( "\"char\"" );
   check< char >( "\"char\"", 'z' );
   check< char >( "\"char\"", 'A' );
   check< char >( "\"char\"", 'Z' );
   check< char >( "\"char\"", '0' );
   check< char >( "\"char\"", '9' );
   check< char >( "\"char\"", '$' );
   check< char >( "\"char\"", '%' );
   check< char >( "\"char\"", ' ' );
   check< char >( "\"char\"", '"' );
   check< char >( "\"char\"", '\'' );
   check< char >( "\"char\"", '\\' );
   check< char >( "\"char\"", '\n' );
   check< char >( "\"char\"", '\t' );

   check< signed char >( "NUMERIC" );
   check< unsigned char >( "NUMERIC" );
   check< short >( "NUMERIC" );
   check< unsigned short >( "NUMERIC" );
   check< int >( "NUMERIC" );
   check< unsigned >( "NUMERIC" );
   check< long >( "NUMERIC" );
   check< unsigned long >( "NUMERIC" );
   check< long long >( "NUMERIC" );
   check< unsigned long long >( "NUMERIC" );

   // this allows precise floating point values
   my_connection->execute( "SET extra_float_digits=3" );

   check_null( "REAL" );
   check< float >( "REAL", std::numeric_limits< float >::lowest() );
   check_null( "REAL" );
   check< float >( "REAL", -1e37F );
   check< float >( "REAL", -1.25F );
   check< float >( "REAL", -1.F );
   check< float >( "REAL", -0.25F );
   check< float >( "REAL", -1e-37F );
   check< float >( "REAL", 0.F );
   check< float >( "REAL", std::numeric_limits< float >::min() );
   check< float >( "REAL", 1e-37F );
   {
      float value = 0.123456F;
      for( int i = 0; i < 32; ++i ) {
         check< float >( "REAL", value );
         value = std::nextafterf( value, 1 );
      }
   }
   check< float >( "REAL", 0.25F );
   check< float >( "REAL", 1.F );
   check< float >( "REAL", 1.F + std::numeric_limits< float >::epsilon() );
   check< float >( "REAL", 1.25F );
   check< float >( "REAL", 1e37F );
   check< float >( "REAL", std::numeric_limits< float >::max() );
   check< float >( "REAL", INFINITY );
   check< float >( "REAL", -INFINITY );
   check< float >( "REAL", NAN );

   check_null( "DOUBLE PRECISION" );
   check< double >( "DOUBLE PRECISION", std::numeric_limits< double >::lowest() );
   check_null( "DOUBLE PRECISION" );
   check< double >( "DOUBLE PRECISION", -1e308 );
   check< double >( "DOUBLE PRECISION", -1.25 );
   check< double >( "DOUBLE PRECISION", -1 );
   check< double >( "DOUBLE PRECISION", -0.25 );
   check< double >( "DOUBLE PRECISION", -1e-307 );
   check< double >( "DOUBLE PRECISION", 0 );
   check< double >( "DOUBLE PRECISION", std::numeric_limits< double >::min() );
   check< double >( "DOUBLE PRECISION", 1e-307 );
   {
      double value = 0.123456789012345;
      for( int i = 0; i < 32; ++i ) {
         check< double >( "DOUBLE PRECISION", value );
         value = std::nextafter( value, 1 );
      }
   }
   check< double >( "DOUBLE PRECISION", 0.25 );
   check< double >( "DOUBLE PRECISION", 1 );
   check< double >( "DOUBLE PRECISION", 1 + std::numeric_limits< double >::epsilon() );
   check< double >( "DOUBLE PRECISION", 1.25 );
   check< double >( "DOUBLE PRECISION", 1e308 );
   check< double >( "DOUBLE PRECISION", std::numeric_limits< double >::max() );
   check< double >( "DOUBLE PRECISION", INFINITY );
   check< double >( "DOUBLE PRECISION", -INFINITY );  // NOLINT(bugprone-narrowing-conversions)
   check< double >( "DOUBLE PRECISION", NAN );

   check_null( "NUMERIC" );
   check< float >( "NUMERIC", std::numeric_limits< float >::lowest() );
   check< float >( "NUMERIC", -1e37F );
   check< float >( "NUMERIC", -1.25F );
   check< float >( "NUMERIC", -1.F );
   check< float >( "NUMERIC", -0.25F );
   check< float >( "NUMERIC", -1e-37F );
   check< float >( "NUMERIC", 0.F );
   check< float >( "NUMERIC", std::numeric_limits< float >::min() );
   check< float >( "NUMERIC", 1e-37F );
   {
      float value = 0.123456F;
      for( int i = 0; i < 32; ++i ) {
         check< float >( "NUMERIC", value );
         value = std::nextafterf( value, 1 );
      }
   }
   check< float >( "NUMERIC", 0.25F );
   check< float >( "NUMERIC", 1.F );
   check< float >( "NUMERIC", 1.F + std::numeric_limits< float >::epsilon() );
   check< float >( "NUMERIC", 1.25F );
   check< float >( "NUMERIC", 1e37F );
   check< float >( "NUMERIC", std::numeric_limits< float >::max() );
   check< float >( "NUMERIC", NAN );

   check< double >( "NUMERIC", std::numeric_limits< double >::lowest() );
   check< double >( "NUMERIC", -1e308 );
   check< double >( "NUMERIC", -1.25 );
   check< double >( "NUMERIC", -1 );
   check< double >( "NUMERIC", -0.25 );
   check< double >( "NUMERIC", -1e-307 );
   check< double >( "NUMERIC", 0 );
   check< double >( "NUMERIC", std::numeric_limits< double >::min() );
   check< double >( "NUMERIC", 1e-307 );
   {
      double value = 0.123456789012345;
      for( int i = 0; i < 32; ++i ) {
         check< double >( "NUMERIC", value );
         value = std::nextafter( value, 1 );
      }
   }
   check< double >( "NUMERIC", 0.25 );
   check< double >( "NUMERIC", 1 );
   check< double >( "NUMERIC", 1 + std::numeric_limits< double >::epsilon() );
   check< double >( "NUMERIC", 1.25 );
   check< double >( "NUMERIC", 1e308 );
   check< double >( "NUMERIC", std::numeric_limits< double >::max() );
   check< double >( "NUMERIC", NAN );

   check< long double >( "NUMERIC", std::numeric_limits< long double >::lowest() );
   check< long double >( "NUMERIC", -1e308 );
   check< long double >( "NUMERIC", -1.25 );
   check< long double >( "NUMERIC", -1 );
   check< long double >( "NUMERIC", -0.25 );
   check< long double >( "NUMERIC", -1e-307 );
   check< long double >( "NUMERIC", 0 );
   check< long double >( "NUMERIC", std::numeric_limits< long double >::min() );
   check< long double >( "NUMERIC", 1e-307 );
   {
      long double value = 0.123456789012345;
      for( int i = 0; i < 32; ++i ) {
         check< long double >( "NUMERIC", value );
         value = std::nextafterl( value, 1 );
      }
   }
   check< long double >( "NUMERIC", 0.25 );
   check< long double >( "NUMERIC", 1 );
   check< long double >( "NUMERIC", 1e-307 );
   check< long double >( "NUMERIC", 1 + std::numeric_limits< long double >::epsilon() );
   check< long double >( "NUMERIC", 1.25 );
   check< long double >( "NUMERIC", 1e308 );
   check< long double >( "NUMERIC", std::numeric_limits< long double >::max() );
   check< long double >( "NUMERIC", NAN );

   check_null( "TEXT" );
   check< float >( "TEXT", std::numeric_limits< float >::lowest() );
   check_null( "TEXT" );
   check< float >( "TEXT", -1e37F );
   check< float >( "TEXT", -1.25F );
   check< float >( "TEXT", -1.F );
   check< float >( "TEXT", -0.25F );
   check< float >( "TEXT", -1e-37F );
   check< float >( "TEXT", 0.F );
   check< float >( "TEXT", std::numeric_limits< float >::min() );
   check< float >( "TEXT", 1e-37F );
   {
      float value = 0.123456F;
      for( int i = 0; i < 32; ++i ) {
         check< float >( "TEXT", value );
         value = std::nextafterf( value, 1 );
      }
   }
   check< float >( "TEXT", 0.25F );
   check< float >( "TEXT", 1.F );
   check< float >( "TEXT", 1.F + std::numeric_limits< float >::epsilon() );
   check< float >( "TEXT", 1.25F );
   check< float >( "TEXT", 1e37F );
   check< float >( "TEXT", std::numeric_limits< float >::max() );
   check< float >( "TEXT", INFINITY );
   check< float >( "TEXT", -INFINITY );
   check< float >( "TEXT", NAN );

   check< double >( "TEXT", std::numeric_limits< double >::lowest() );
   check< double >( "TEXT", -1e308 );
   check< double >( "TEXT", -1.25 );
   check< double >( "TEXT", -1 );
   check< double >( "TEXT", -0.25 );
   check< double >( "TEXT", -1e-307 );
   check< double >( "TEXT", 0 );
   check< double >( "TEXT", std::numeric_limits< double >::min() );
   check< double >( "TEXT", 1e-307 );
   {
      double value = 0.123456789012345;
      for( int i = 0; i < 32; ++i ) {
         check< double >( "TEXT", value );
         value = std::nextafter( value, 1 );
      }
   }
   check< double >( "TEXT", 0.25 );
   check< double >( "TEXT", 1 );
   check< double >( "TEXT", 1 + std::numeric_limits< double >::epsilon() );
   check< double >( "TEXT", 1.25 );
   check< double >( "TEXT", 1e308 );
   check< double >( "TEXT", std::numeric_limits< double >::max() );
   check< double >( "TEXT", INFINITY );
   check< double >( "TEXT", -INFINITY );  // NOLINT(bugprone-narrowing-conversions)
   check< double >( "TEXT", NAN );

   // there is no data type to store 'long double' to PostgreSQL - but TEXT should do just fine...
   check< long double >( "TEXT", std::numeric_limits< long double >::lowest() );
   check< long double >( "TEXT", -1e308 );
   check< long double >( "TEXT", -1.25 );
   check< long double >( "TEXT", -1 );
   check< long double >( "TEXT", -0.25 );
   check< long double >( "TEXT", -1e-307 );
   check< long double >( "TEXT", 0 );
   check< long double >( "TEXT", std::numeric_limits< long double >::min() );
   check< long double >( "TEXT", 1e-307 );
   {
      long double value = 0.123456789012345;
      for( int i = 0; i < 32; ++i ) {
         check< long double >( "TEXT", value );
         value = std::nextafterl( value, 1 );
      }
   }
   check< long double >( "TEXT", 0.25 );
   check< long double >( "TEXT", 1 );
   check< long double >( "TEXT", 1e-307 );
   check< long double >( "TEXT", 1 + std::numeric_limits< long double >::epsilon() );
   check< long double >( "TEXT", 1.25 );
   check< long double >( "TEXT", 1e308 );
   check< long double >( "TEXT", std::numeric_limits< long double >::max() );
   check< long double >( "TEXT", INFINITY );
   check< long double >( "TEXT", -INFINITY );  // NOLINT(bugprone-narrowing-conversions)
   check< long double >( "TEXT", NAN );

   check< std::string >( "TEXT", "" );
   check< std::string >( "TEXT", " " );
   check< std::string >( "TEXT", "abc" );
   check< std::string >( "TEXT", "Hello, world!" );
   check< std::string >( "TEXT", ";" );
   check< std::string >( "TEXT", "\\" );
   check< std::string >( "TEXT", "\"" );
   check< std::string >( "TEXT", "'" );
   check< std::string >( "TEXT", "'; DROP TABLE users; --" );
   check< std::string >( "TEXT", "ä" );
   check< std::string >( "TEXT", "€" );
   check< std::string >( "TEXT", "𝄞" );
   check< std::string >( "TEXT", "äöüÄÖÜß€𝄞" );
   check< std::string >( "TEXT", "ä\tö\nü\1Ä\"Ö;Ü'ß#€𝄞" );

   check_null( "BYTEA" );

   const unsigned char bdata[] = { 'v', 255, 0, 'a', 1, 'b', 0 };

   check_bytea( std::basic_string< unsigned char >( bdata, 7 ) );
   check_bytea( std::basic_string_view< unsigned char >( bdata, 7 ) );

   check_bytea( tao::pq::to_binary( bdata ) );
   check_bytea( tao::pq::to_binary_view( bdata ) );
}

auto main() -> int
{
   try {
      run();
   }
   // LCOV_EXCL_START
   catch( const std::exception& e ) {
      std::cerr << "exception: " << e.what() << std::endl;
      throw;
   }
   catch( ... ) {
      std::cerr << "unknown exception" << std::endl;
      throw;
   }
   // LCOV_EXCL_STOP
}
