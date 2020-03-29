// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>

std::shared_ptr< tao::pq::connection > connection;

namespace tao::pq
{
   class bytea  // NOLINT
   {
   private:
      std::size_t m_size;
      unsigned char* m_data;

   public:
      explicit bytea( const char* value )
         : m_size( 0 ), m_data( PQunescapeBytea( (unsigned char*)value, &m_size ) )  //NOLINT
      {
         if( m_data == nullptr ) {
            throw std::bad_alloc();
         }
      }

      ~bytea()
      {
         PQfreemem( m_data );
      }

      bytea( const bytea& ) = delete;
      auto operator=( const bytea& ) -> bytea& = delete;

      [[nodiscard]] auto size() const noexcept
      {
         return m_size;
      }

      [[nodiscard]] auto data() const noexcept -> const unsigned char*
      {
         return m_data;
      }

      [[nodiscard]] auto operator[]( const std::size_t idx ) const noexcept -> unsigned char
      {
         return m_data[ idx ];
      }
   };

   template<>
   struct result_traits< bytea >
   {
      [[nodiscard]] static auto from( const char* value )
      {
         return bytea( value );
      }
   };

}  // namespace tao::pq

auto prepare_datatype( const std::string& datatype ) -> bool
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
      TEST_ASSERT( connection->execute( "INSERT INTO tao_basic_datatypes_test VALUES ( $1 )", tao::pq::null ).rows_affected() == 1 );
   }
   else {
      TEST_ASSERT( connection->execute( "UPDATE tao_basic_datatypes_test SET a=$1", tao::pq::null ).rows_affected() == 1 );
   }
   const auto result = connection->execute( "SELECT * FROM tao_basic_datatypes_test" );
   TEST_ASSERT( result[ 0 ][ 0 ].is_null() );
}

template< typename T >
void check( const std::string& datatype, const T& value )
{
   std::cout << "check: " << datatype << " value: " << value << std::endl;
   if( prepare_datatype( datatype ) ) {
      TEST_ASSERT( connection->execute( "INSERT INTO tao_basic_datatypes_test VALUES ( $1 )", value ).rows_affected() == 1 );
   }
   else {
      TEST_ASSERT( connection->execute( "UPDATE tao_basic_datatypes_test SET a=$1", value ).rows_affected() == 1 );
   }
   const auto result = connection->execute( "SELECT * FROM tao_basic_datatypes_test" );
   if( value == value ) {  // NOLINT(misc-redundant-expression)
      if( result[ 0 ][ 0 ].as< T >() != value ) {
         std::cout << "check: " << datatype << " value: " << value << " result: " << result.get( 0, 0 ) << " FAILED!" << std::endl;
         TEST_ASSERT( false );
      }
   }
   else {
      const auto v = result[ 0 ][ 0 ].as< T >();
      TEST_ASSERT( v != v );
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

template< template< typename... > class Traits, typename T >
void check_bytea( T&& t )
{
   TEST_ASSERT( connection->execute< Traits >( "UPDATE tao_basic_datatypes_test SET a=$1", tao::span( std::forward< T >( t ) ) ).rows_affected() == 1 );

   const auto result = connection->execute( "SELECT * FROM tao_basic_datatypes_test" )[ 0 ][ 0 ].as< tao::pq::bytea >();
   TEST_ASSERT( result.size() == 7 );
   TEST_ASSERT( result[ 0 ] == static_cast< unsigned char >( t[ 0 ] ) );
   TEST_ASSERT( result[ 1 ] == static_cast< unsigned char >( t[ 1 ] ) );
   TEST_ASSERT( result[ 2 ] == static_cast< unsigned char >( t[ 2 ] ) );
   TEST_ASSERT( result[ 3 ] == static_cast< unsigned char >( t[ 3 ] ) );
   TEST_ASSERT( result[ 4 ] == static_cast< unsigned char >( t[ 4 ] ) );
   TEST_ASSERT( result[ 5 ] == static_cast< unsigned char >( t[ 5 ] ) );
   TEST_ASSERT( result[ 6 ] == static_cast< unsigned char >( t[ 6 ] ) );
}

template< typename T >
void check_bytea( T&& t )
{
   check_bytea< tao::pq::parameter_text_traits >( std::forward< T >( t ) );
   check_bytea< tao::pq::parameter_binary_traits >( std::forward< T >( t ) );
}

void run()
{
   connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );

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
   check< double >( "DOUBLE PRECISION", -INFINITY );
   check< double >( "DOUBLE PRECISION", NAN );

   // check< float >( "NUMERIC", std::numeric_limits< float >::lowest() );
   check< float >( "NUMERIC", -1e37F );
   check< float >( "NUMERIC", -1.25F );
   check< float >( "NUMERIC", -1.F );
   check< float >( "NUMERIC", -0.25F );
   check< float >( "NUMERIC", -1e-37F );
   check< float >( "NUMERIC", 0.F );
   // check< float >( "NUMERIC", std::numeric_limits< float >::min() );
   check< float >( "NUMERIC", 1e-37F );
   // {
   //    float value = 0.123456F;
   //    for( int i = 0; i < 32; ++i ) {
   //       check< float >( "NUMERIC", value );
   //       value = std::nextafterf( value, 1 );
   //    }
   // }
   check< float >( "NUMERIC", 0.25F );
   check< float >( "NUMERIC", 1.F );
   // check< float >( "NUMERIC", 1.F + std::numeric_limits< float >::epsilon() );
   check< float >( "NUMERIC", 1.25F );
   check< float >( "NUMERIC", 1e37F );
   // check< float >( "NUMERIC", std::numeric_limits< float >::max() );
   check< float >( "NUMERIC", NAN );

   // check< double >( "NUMERIC", std::numeric_limits< double >::lowest() );
   check< double >( "NUMERIC", -1e308 );
   check< double >( "NUMERIC", -1.25 );
   check< double >( "NUMERIC", -1 );
   check< double >( "NUMERIC", -0.25 );
   check< double >( "NUMERIC", -1e-307 );
   check< double >( "NUMERIC", 0 );
   // check< double >( "NUMERIC", std::numeric_limits< double >::min() );
   check< double >( "NUMERIC", 1e-307 );
   // {
   //    double value = 0.123456789012345;
   //    for( int i = 0; i < 32; ++i ) {
   //       check< double >( "NUMERIC", value );
   //       value = std::nextafter( value, 1 );
   //    }
   // }
   check< double >( "NUMERIC", 0.25 );
   check< double >( "NUMERIC", 1 );
   // check< double >( "NUMERIC", 1 + std::numeric_limits< double >::epsilon() );
   check< double >( "NUMERIC", 1.25 );
   check< double >( "NUMERIC", 1e308 );
   // check< double >( "NUMERIC", std::numeric_limits< double >::max() );
   check< double >( "NUMERIC", NAN );

   check< long double >( "NUMERIC", std::numeric_limits< long double >::lowest() );
#ifndef _MSC_VER
   check< long double >( "NUMERIC", -1e1001L );
   check< long double >( "NUMERIC", -1e1000L );
   check< long double >( "NUMERIC", -1e999L );
#endif
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
#ifndef _MSC_VER
   check< long double >( "NUMERIC", 1e999L );
   check< long double >( "NUMERIC", 1e1000L );
   check< long double >( "NUMERIC", 1e1001L );
#endif
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

   // use std::span / tao::span to pass binary data as parameters (works for char, signed char, unsigned char, and std::byte)

#if defined( __clang__ ) && ( __clang_major__ <= 5 )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

   check_null( "BYTEA" );
   {
      char bdata[] = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      const char bdata[] = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::array< char, 7 > bdata = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::array< const char, 7 > bdata = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::vector< char > bdata = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      signed char bdata[] = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      const signed char bdata[] = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::array< signed char, 7 > bdata = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::array< const signed char, 7 > bdata = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::vector< signed char > bdata = { 'x', 1, 42, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      unsigned char bdata[] = { 'x', 1, 255, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      const unsigned char bdata[] = { 'x', 1, 255, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::array< unsigned char, 7 > bdata = { 'x', 1, 255, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::array< const unsigned char, 7 > bdata = { 'x', 1, 255, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::vector< unsigned char > bdata = { 'x', 1, 255, 0, 'a', 0, 'b' };
      check_bytea( bdata );
   }

   {
      std::byte bdata[ 7 ] = { std::byte( 'v' ), std::byte( 255 ), std::byte( 0 ), std::byte( 'a' ), std::byte( 1 ), std::byte( 'b' ), std::byte( 0 ) };
      check_bytea( bdata );
   }

   {
      const std::byte bdata[ 7 ] = { std::byte( 'v' ), std::byte( 255 ), std::byte( 0 ), std::byte( 'a' ), std::byte( 1 ), std::byte( 'b' ), std::byte( 0 ) };
      check_bytea( bdata );
   }

   {
      std::array< std::byte, 7 > bdata = { std::byte( 'v' ), std::byte( 255 ), std::byte( 0 ), std::byte( 'a' ), std::byte( 1 ), std::byte( 'b' ), std::byte( 0 ) };
      check_bytea( bdata );
   }

   {
      std::array< const std::byte, 7 > bdata = { std::byte( 'v' ), std::byte( 255 ), std::byte( 0 ), std::byte( 'a' ), std::byte( 1 ), std::byte( 'b' ), std::byte( 0 ) };
      check_bytea( bdata );
   }

   {
      std::vector< std::byte > bdata = { std::byte( 'v' ), std::byte( 255 ), std::byte( 0 ), std::byte( 'a' ), std::byte( 1 ), std::byte( 'b' ), std::byte( 0 ) };
      check_bytea( bdata );
   }

#if defined( __clang__ ) && ( __clang_major__ <= 5 )
#pragma clang diagnostic pop
#endif
}

auto main() -> int
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
