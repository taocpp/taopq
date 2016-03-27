// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>

#include <tao/utility/getenv.hpp>
#include <tao/utility/demangle.hpp>
#include <tao/postgres/connection.hpp>

std::shared_ptr< tao::postgres::connection > connection;

bool prepare_datatype( const std::string& datatype )
{
  static std::string last;
  if( datatype == last ) {
    return false;
  }
  last = datatype;
  connection->execute( "DROP TABLE IF EXISTS tao_basic_datatypes_test" );
  connection->execute( "CREATE TABLE tao_basic_datatypes_test ( a " + datatype + " )" );
  return true;
}

void check_null( const std::string& datatype )
{
  std::cout << "check null: " << datatype << std::endl;
  if( prepare_datatype( datatype ) ) {
    assert( connection->execute( "INSERT INTO tao_basic_datatypes_test VALUES ( $1 )", tao::postgres::null ).rows_affected() == 1 );
  }
  else {
    assert( connection->execute( "UPDATE tao_basic_datatypes_test SET a=$1", tao::postgres::null ).rows_affected() == 1 );
  }
  const auto result = connection->execute( "SELECT * FROM tao_basic_datatypes_test" );
  assert( result[ 0 ][ 0 ].is_null() );
}

template< typename T >
void check( const std::string& datatype, const T& value )
{
  std::cout << "check: " << datatype << " value: " << value << std::endl;
  if( prepare_datatype( datatype ) ) {
    assert( connection->execute( "INSERT INTO tao_basic_datatypes_test VALUES ( $1 )", value ).rows_affected() == 1 );
  }
  else {
    assert( connection->execute( "UPDATE tao_basic_datatypes_test SET a=$1", value ).rows_affected() == 1 );
  }
  const auto result = connection->execute( "SELECT * FROM tao_basic_datatypes_test" );
  if( value == value ) {
    assert( result[ 0 ][ 0 ].as< T >() == value );
  }
  else {
    const auto value = result[ 0 ][ 0 ].as< T >();
    assert( value != value );
  }
}

template< typename T >
typename std::enable_if< std::is_signed< T >::value >::type check( const std::string& datatype )
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
typename std::enable_if< std::is_unsigned< T >::value >::type check( const std::string& datatype )
{
  check_null( datatype );
  check< T >( datatype, 0 );
  check_null( datatype );
  check< T >( datatype, 1 );
  check< T >( datatype, 42 );
  check< T >( datatype, std::numeric_limits< T >::max() );
}

int main()
{
  connection = tao::postgres::connection::create( tao::utility::getenv( "TAO_TEST_DATABASE", "dbname=template1" ) );

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
  connection->execute( "SET extra_float_digits=3" );

  check_null( "REAL" );
  check< float >( "REAL", std::numeric_limits< float >::lowest() );
  check_null( "REAL" );
  check< float >( "REAL", -1e37 );
  check< float >( "REAL", -1.25 );
  check< float >( "REAL", -1 );
  check< float >( "REAL", -0.25 );
  check< float >( "REAL", -1e-37 );
  check< float >( "REAL", 0 );
  // check< float >( "REAL", std::numeric_limits< float >::min() );
  check< float >( "REAL", 1e-37 );
  {
    float value = 0.123456f;
    for( int i = 0; i < 32; ++i ) {
      check< float >( "REAL", value );
      value = std::nextafterf( value, 1 );
    }
  }
  check< float >( "REAL", 0.25 );
  check< float >( "REAL", 1 );
  check< float >( "REAL", 1 + std::numeric_limits< float >::epsilon() );
  check< float >( "REAL", 1.25 );
  check< float >( "REAL", 1e37 );
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
  // check< double >( "DOUBLE PRECISION", std::numeric_limits< double >::min() );
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
  check< double >( "DOUBLE PRECISION", -INFINITY );
  check< double >( "DOUBLE PRECISION", NAN );

  check< float >( "NUMERIC", std::numeric_limits< float >::lowest() );
  check< float >( "NUMERIC", -1e37 );
  check< float >( "NUMERIC", -1.25 );
  check< float >( "NUMERIC", -1 );
  check< float >( "NUMERIC", -0.25 );
  check< float >( "NUMERIC", -1e-37 );
  check< float >( "NUMERIC", 0 );
  // check< float >( "NUMERIC", std::numeric_limits< float >::min() );
  check< float >( "NUMERIC", 1e-37 );
  {
    float value = 0.123456f;
    for( int i = 0; i < 32; ++i ) {
      check< float >( "NUMERIC", value );
      value = std::nextafterf( value, 1 );
    }
  }
  check< float >( "NUMERIC", 0.25 );
  check< float >( "NUMERIC", 1 );
  check< float >( "NUMERIC", 1 + std::numeric_limits< float >::epsilon() );
  check< float >( "NUMERIC", 1.25 );
  check< float >( "NUMERIC", 1e37 );
  check< float >( "NUMERIC", std::numeric_limits< float >::max() );
  check< float >( "NUMERIC", NAN );

  check< double >( "NUMERIC", std::numeric_limits< double >::lowest() );
  check< double >( "NUMERIC", -1e308 );
  check< double >( "NUMERIC", -1.25 );
  check< double >( "NUMERIC", -1 );
  check< double >( "NUMERIC", -0.25 );
  check< double >( "NUMERIC", -1e-307 );
  check< double >( "NUMERIC", 0 );
  // check< double >( "NUMERIC", std::numeric_limits< double >::min() );
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
  check< long double >( "NUMERIC", -1e1001L );
  check< long double >( "NUMERIC", -1e1000L );
  check< long double >( "NUMERIC", -1e999L );
  check< long double >( "NUMERIC", -1e308 );
  check< long double >( "NUMERIC", -1.25 );
  check< long double >( "NUMERIC", -1 );
  check< long double >( "NUMERIC", -0.25 );
  check< long double >( "NUMERIC", -1e-307 );
  check< long double >( "NUMERIC", 0 );
  // check< long double >( "NUMERIC", std::numeric_limits< long double >::min() );
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
  check< long double >( "NUMERIC", 1e999L );
  check< long double >( "NUMERIC", 1e1000L );
  check< long double >( "NUMERIC", 1e1001L );
  check< long double >( "NUMERIC", std::numeric_limits< long double >::max() );
  check< long double >( "NUMERIC", NAN );

  check_null( "TEXT" );
  check< float >( "TEXT", std::numeric_limits< float >::lowest() );
  check_null( "TEXT" );
  check< float >( "TEXT", -1e37 );
  check< float >( "TEXT", -1.25 );
  check< float >( "TEXT", -1 );
  check< float >( "TEXT", -0.25 );
  check< float >( "TEXT", -1e-37 );
  check< float >( "TEXT", 0 );
  check< float >( "TEXT", std::numeric_limits< float >::min() );
  check< float >( "TEXT", 1e-37 );
  {
    float value = 0.123456f;
    for( int i = 0; i < 32; ++i ) {
      check< float >( "TEXT", value );
      value = std::nextafterf( value, 1 );
    }
  }
  check< float >( "TEXT", 0.25 );
  check< float >( "TEXT", 1 );
  check< float >( "TEXT", 1 + std::numeric_limits< float >::epsilon() );
  check< float >( "TEXT", 1.25 );
  check< float >( "TEXT", 1e37 );
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
  check< double >( "TEXT", -INFINITY );
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
  check< long double >( "TEXT", -INFINITY );
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
}
