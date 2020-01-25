// Copyright (c) 2019 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>
#include <tao/pq/result_traits_tuple.hpp>

class user
{
private:
   int a, b, c, d;

public:
   explicit user( int i )
      : a( i ), b( i + 1 ), c( i + 2 ), d( i + 3 )
   {}

   friend auto to_taopq_param( const user& u ) noexcept
   {
      return std::tie( u.a, u.b, u.c, u.d );
   }
};

void run()
{
   const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );

   connection->execute( "DROP TABLE IF EXISTS tao_parameter_test" );
   connection->execute( "CREATE TABLE tao_parameter_test ( a INTEGER PRIMARY KEY, b INTEGER, c INTEGER, d INTEGER )" );

   TEST_EXECUTE( connection->execute( "INSERT INTO tao_parameter_test VALUES ( 1, 2, 3, 4 )" ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_parameter_test VALUES ( $1, $2, $3, $4 )", 2, 3, 4, 5 ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_parameter_test VALUES ( $1, $2, $3, $4 )", 3, std::make_tuple( 4, 5 ), 6 ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_parameter_test VALUES ( $1, $2, $3, $4 )", std::make_tuple( 4, 5 ), std::make_tuple( 6, 7 ) ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_parameter_test VALUES ( $1, $2, $3, $4 )", std::make_tuple( 5, std::make_tuple( 6, 7 ), 8 ) ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_parameter_test VALUES ( $1, $2, $3, $4 )", user( 6 ) ) );

   const auto result = connection->execute( "SELECT * FROM tao_parameter_test" );
   TEST_ASSERT( result.size() == 6 );

   for( const auto& row : result ) {
      {
         const auto v = row.tuple< int, int, int, int >();
         TEST_ASSERT( std::get< 1 >( v ) == std::get< 0 >( v ) + 1 );
         TEST_ASSERT( std::get< 2 >( v ) == std::get< 1 >( v ) + 1 );
         TEST_ASSERT( std::get< 3 >( v ) == std::get< 2 >( v ) + 1 );
      }
      {
         const auto v = row.tuple< std::tuple< int, int >, std::tuple< int, int > >();
         TEST_ASSERT( std::get< 1 >( std::get< 0 >( v ) ) == std::get< 0 >( std::get< 0 >( v ) ) + 1 );
         TEST_ASSERT( std::get< 0 >( std::get< 1 >( v ) ) == std::get< 1 >( std::get< 0 >( v ) ) + 1 );
         TEST_ASSERT( std::get< 1 >( std::get< 1 >( v ) ) == std::get< 0 >( std::get< 1 >( v ) ) + 1 );
      }
      {
         const auto v = row.tuple< int, std::tuple< int, int >, int >();
         TEST_ASSERT( std::get< 0 >( std::get< 1 >( v ) ) == std::get< 0 >( v ) + 1 );
         TEST_ASSERT( std::get< 1 >( std::get< 1 >( v ) ) == std::get< 0 >( std::get< 1 >( v ) ) + 1 );
         TEST_ASSERT( std::get< 2 >( v ) == std::get< 1 >( std::get< 1 >( v ) ) + 1 );
      }
      {
         const auto [ a, b, c, d ] = row.tuple< int, int, int, int >();
         TEST_ASSERT( b == a + 1 );
         TEST_ASSERT( c == b + 1 );
         TEST_ASSERT( d == c + 1 );
      }
   }
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
