// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <array>
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <tao/pq.hpp>

namespace
{
   void run()
   {
      // overwrite the default with an environment variable if needed
      const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );
      const auto connection = tao::pq::connection::create( connection_string );

      {
         connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
         connection->execute( "CREATE TABLE tao_array_test ( a INTEGER[] )" );

         const std::array v{ 1, 0, 2, 4 };
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", v );

         std::vector v2 = { 42, 1701 };
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", v2 );

         v2.clear();
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", v2 );

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

         const std::vector< std::optional< std::string > > v = { "FOO", "", "{BAR\\BAZ\"B,L;A}", "NULL", std::nullopt };
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", v );

         const auto r = connection->execute( "SELECT * FROM tao_array_test" ).as< std::vector< std::optional< std::string > > >();
         TEST_ASSERT( r.size() == 5 );
         TEST_ASSERT( r[ 0 ] == "FOO" );
         TEST_ASSERT( r[ 1 ]->empty() );  // NOLINT(bugprone-unchecked-optional-access)
         TEST_ASSERT( r[ 2 ] == "{BAR\\BAZ\"B,L;A}" );
         TEST_ASSERT( r[ 3 ] == "NULL" );
         TEST_ASSERT( !r[ 4 ] );
      }

      {
         connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
         connection->execute( "CREATE TABLE tao_array_test ( a TEXT[][] NOT NULL )" );

         const std::vector< std::vector< std::string > > v = { { "1", "F\"O\\O", "NULL" }, { "4", " XYZ ", "6" } };
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", v );

         const auto r = connection->execute( "SELECT * FROM tao_array_test" ).as< std::vector< std::vector< std::string > > >();
         TEST_ASSERT( r == v );
      }

      {
         connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
         connection->execute( "CREATE TABLE tao_array_test ( a BYTEA[][] NOT NULL )" );

         const std::vector< tao::pq::binary > v = { tao::pq::to_binary( "1" ),
                                                    tao::pq::binary(),
                                                    tao::pq::to_binary( "F\"O\\O" ),
                                                    tao::pq::to_binary( "NU\0LL" ) };
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", v );

         const auto r = connection->execute( "SELECT * FROM tao_array_test" ).as< std::vector< tao::pq::binary > >();
         // TEST_ASSERT( r == v );
      }

      TEST_THROWS( connection->execute( "SELECT $1", "" ).as< std::vector< std::string > >() );
      TEST_THROWS( connection->execute( "SELECT $1", "{" ).as< std::vector< std::string > >() );
      TEST_THROWS( connection->execute( "SELECT $1", "{FOO" ).as< std::vector< std::string > >() );
      TEST_THROWS( connection->execute( "SELECT $1", "{NULL}" ).as< std::vector< std::string > >() );
      TEST_THROWS( connection->execute( "SELECT $1", "{\"FOO}" ).as< std::vector< std::string > >() );
      TEST_THROWS( connection->execute( "SELECT $1", "{FOO}BAR" ).as< std::vector< std::string > >() );

      {
         connection->execute( "DROP TABLE IF EXISTS tao_array_test" );
         connection->execute( "CREATE TABLE tao_array_test ( a INTEGER NOT NULL )" );

         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", 1 );
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", 2 );
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", 3 );
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", 4 );
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", 5 );
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", 6 );
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", 7 );
         connection->execute( "INSERT INTO tao_array_test VALUES ( $1 )", 8 );

         const auto result = connection->execute( "SELECT * FROM tao_array_test WHERE a = ANY( $1 )", std::array{ 2, 3, 5, 6, 9 } );
         TEST_ASSERT( result.vector< int >() == std::vector< int >{ 2, 3, 5, 6 } );
      }
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
