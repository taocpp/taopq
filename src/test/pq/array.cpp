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

   connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
   connection->execute( "CREATE TABLE tao_array_test ( a INTEGER[] )" );

   std::array< int, 4 > v{ 1, 0, 2, 4 };
   connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v );

   std::vector< int > v2 = { 42, 1701 };
   connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v2 );

   v2.clear();
   connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v2 );

   connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", tao::pq::null );

   for( const auto& s : connection->execute( "SELECT * FROM tao_array_test" ).vector< std::optional< std::string > >() ) {
      std::cout << ( !s ? "NULL" : ( '"' + *s + '"' ) ) << std::endl;
   }

   connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
   connection->execute( "CREATE TABLE tao_array_test ( a TEXT[] )" );

   std::vector< std::optional< std::string > > v3 = { "FOO", "", "BAR\\BAZ\"BLA", "NULL", std::nullopt };
   connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v3 );

   for( const auto& s : connection->execute( "SELECT * FROM tao_array_test" ).vector< std::optional< std::string > >() ) {
      std::cout << ( !s ? "NULL" : ( '"' + *s + '"' ) ) << std::endl;
   }

   connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
   connection->execute( "CREATE TABLE tao_array_test ( a TEXT[][] NOT NULL )" );

   std::vector< std::vector< std::string > > v4 = { { "1", "2", "3" }, { "4", "5", "6" } };
   connection->execute( "INSERT INTO tao_array_test VALUES ( $1 ) ", v4 );

   for( const auto& s : connection->execute( "SELECT * FROM tao_array_test" ).vector< std::string >() ) {
      std::cout << '"' << s << '"' << std::endl;
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
