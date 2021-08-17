// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq.hpp>

void run()
{
   // overwrite the default with an environment variable if needed
   const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );
   const auto connection = tao::pq::connection::create( connection_string );

   {
      connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
      connection->execute( "CREATE TABLE tao_array_test ( a INTEGER[] )" );

      std::array< int, 4 > v{ 1, 0, 2, 4 };
      connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v );

      std::vector< int > v2 = { 42, 1701 };
      connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v2 );

      v2.clear();
      connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v2 );

      const auto r = connection->execute( "SELECT * FROM tao_array_test" ).vector< std::vector< int > >();
      TEST_ASSERT( r.size() == 3 );
      TEST_ASSERT( r[ 0 ].size() == 4 );
      TEST_ASSERT( r[ 0 ][ 0 ] == 1 );
      TEST_ASSERT( r[ 0 ][ 1 ] == 0 );
      TEST_ASSERT( r[ 0 ][ 2 ] == 2 );
      TEST_ASSERT( r[ 0 ][ 3 ] == 4 );
      TEST_ASSERT( r[ 1 ].size() == 2 );
      TEST_ASSERT( r[ 1 ][ 0 ] == 42 );
      TEST_ASSERT( r[ 1 ][ 1 ] == 1701 );
      TEST_ASSERT( r[ 2 ].empty() );
   }

   {
      connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
      connection->execute( "CREATE TABLE tao_array_test ( a TEXT[] )" );

      std::vector< std::optional< std::string > > v = { "FOO", "", "{BAR\\BAZ\"B,L;A}", "NULL", std::nullopt };
      connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v );

      const auto r = connection->execute( "SELECT * FROM tao_array_test" ).as< std::vector< std::optional< std::string > > >();
      TEST_ASSERT( r.size() == 5 );
      TEST_ASSERT( *r[ 0 ] == "FOO" );
      TEST_ASSERT( r[ 1 ]->empty() );
      TEST_ASSERT( *r[ 2 ] == "{BAR\\BAZ\"B,L;A}" );
      TEST_ASSERT( *r[ 3 ] == "NULL" );
      TEST_ASSERT( !r[ 4 ] );
   }

   {
      connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
      connection->execute( "CREATE TABLE tao_array_test ( a TEXT[][] NOT NULL )" );

      std::vector< std::vector< std::string > > v = { { "1", "F\"O\\O", "NULL" }, { "4", " XYZ ", "6" } };
      connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v );

      const auto r = connection->execute( "SELECT * FROM tao_array_test" ).as< std::vector< std::vector< std::string > > >();
      TEST_ASSERT( r == v );
   }
}

auto main() -> int  // NOLINT(bugprone-exception-escape)
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