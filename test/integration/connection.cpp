// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "utils/getenv.hpp"
#include "utils/macros.hpp"

#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include <tuple>

#include <tao/pq.hpp>
#include <tao/pq/internal/unreachable.hpp>

namespace
{
   // LCOV_EXCL_START
   auto my_poll( const int /*unused*/, const bool /*unused*/, const int /*unused*/ ) -> tao::pq::poll::status
   {
      TAO_PQ_INTERNAL_UNREACHABLE;
   }
   // LCOV_EXCL_STOP

   void run()
   {
      using namespace std::chrono_literals;

      // overwrite the default with an environment variable if needed
      const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );  // NOLINT(clang-analyzer-deadcode.DeadStores)

      // connection_string must be valid
      TEST_THROWS( tao::pq::connection::create( "=" ) );

      // connection_string must reference an existing and accessible database
      TEST_THROWS( tao::pq::connection::create( "dbname=DOES_NOT_EXIST" ) );

      // open a connection
      const auto connection = tao::pq::connection::create( connection_string );
      connection->set_timeout( 1s );

      // open a second, independent connection (and discard it immediately)
      std::ignore = tao::pq::connection::create( connection_string );

      // execute an SQL statement
      connection->execute( "DROP TABLE IF EXISTS tao_connection_test" );

      TEST_THROWS( connection->direct()->get_result() );

      // execution of empty statements fails
      TEST_THROWS( connection->execute( "" ) );

      // execution of invalid statements fails
      TEST_THROWS( connection->execute( "FOO BAR BAZ" ) );

      // a prepared statement's name must be a valid C++ identifier
      TEST_THROWS( connection->prepare( "", "DROP TABLE IF EXISTS tao_connection_test" ) );
      TEST_THROWS( connection->prepare( "0drop_table", "DROP TABLE IF EXISTS tao_connection_test" ) );
      TEST_THROWS( connection->prepare( "drop table", "DROP TABLE IF EXISTS tao_connection_test" ) );

      // prepare a statement
      connection->prepare( "drop_table", "DROP TABLE IF EXISTS tao_connection_test" );

      // execute a prepared statement
      //
      // note: the name of a prepared statement must be a valid identifier, all
      // actual SQL statements can be writen in a form which does not match a valid
      // identifier, so you can always make sure that they can not be confused.
      connection->execute( "drop_table" );

      // statements must consume all parameters
      TEST_THROWS( connection->execute( "drop_table", 42 ) );

      // a statement which is not a query does not return "affected rows"
      TEST_ASSERT( connection->execute( "drop_table" ).columns() == 0 );

      // deallocate a prepared statement
      connection->deallocate( "drop_table" );

      // no longer possible
      TEST_THROWS( connection->execute( "drop_table" ) );

      // deallocate must refer to a prepared statement
      TEST_THROWS( connection->deallocate( "drop_table" ) );

      // deallocate must get a valid name
      TEST_THROWS( connection->deallocate( "FOO BAR" ) );

      // test that prepared statement names are case sensitive
      connection->prepare( "a", "SELECT 1" );
      connection->prepare( "A", "SELECT 2" );

      TEST_THROWS( connection->prepare( "a", "SELECT 2" ) );

      TEST_ASSERT_MESSAGE( "checking prepared statement 'a'", connection->execute( "a" ).as< int >() == 1 );

      connection->deallocate( "a" );

      TEST_ASSERT_MESSAGE( "checking prepared statement 'A'", connection->execute( "A" ).as< int >() == 2 );

      connection->prepare( "a", "SELECT 3" );

      TEST_ASSERT_MESSAGE( "checking prepared statement 'a'", connection->execute( "a" ).as< int >() == 3 );
      TEST_ASSERT_MESSAGE( "checking prepared statement 'A'", connection->execute( "A" ).as< int >() == 2 );

      connection->deallocate( "A" );

      TEST_ASSERT_MESSAGE( "checking prepared statement 'a'", connection->execute( "a" ).as< int >() == 3 );

      connection->prepare( "A", "SELECT 4" );

      TEST_ASSERT_MESSAGE( "checking prepared statement 'a'", connection->execute( "a" ).as< int >() == 3 );
      TEST_ASSERT_MESSAGE( "checking prepared statement 'A'", connection->execute( "A" ).as< int >() == 4 );

      // create a test table
      connection->execute( "CREATE TABLE tao_connection_test ( a INTEGER PRIMARY KEY, b INTEGER )" );

      // a DELETE statement does not yield a result set
      TEST_ASSERT( connection->execute( "DELETE FROM tao_connection_test" ).columns() == 0 );

      // out of range access throws
      TEST_THROWS( connection->execute( "SELECT * FROM tao_connection_test" ).at( 0 ) );

      // insert some data
      connection->execute( "INSERT INTO tao_connection_test VALUES ( 1, 42 )" );

      TEST_THROWS( connection->execute( "COPY tao_connection_test ( a, b ) TO STDOUT" ) );
      TEST_THROWS( connection->execute( "COPY tao_connection_test ( a, b ) FROM STDIN" ) );

      // read data
      TEST_ASSERT( connection->execute( "SELECT b FROM tao_connection_test WHERE a = 1" )[ 0 ][ 0 ].get() == std::string( "42" ) );

      TEST_THROWS( connection->execute( "SELECT $1" ) );
      TEST_THROWS( connection->execute( "SELECT $1", "One", "Two" ) );

      TEST_THROWS( connection->execute( "SELECT $1", "" ).as< tao::pq::binary >() );
      TEST_THROWS( connection->execute( "SELECT $1", "\\" ).as< tao::pq::binary >() );
      TEST_THROWS( connection->execute( "SELECT $1", "\\xa" ).as< tao::pq::binary >() );
      TEST_THROWS( connection->execute( "SELECT $1", "\\xa." ).as< tao::pq::binary >() );

      {
         using callback_t = tao::pq::poll::status ( * )( int, bool, int );

         const auto old_cb = *connection->poll_callback().target< callback_t >();
         TEST_ASSERT( old_cb != nullptr );
         TEST_ASSERT( *connection->poll_callback().target< callback_t >() != &my_poll );
         connection->set_poll_callback( my_poll );
         TEST_ASSERT( *connection->poll_callback().target< callback_t >() == &my_poll );
         connection->reset_poll_callback();
         TEST_ASSERT( *connection->poll_callback().target< callback_t >() == old_cb );
      }

      connection->reset_timeout();
      TEST_EXECUTE( connection->execute( "SELECT pg_sleep( 0.2 )" ) );

      connection->set_timeout( 100ms );
      TEST_THROWS( connection->execute( "SELECT pg_sleep( .2 )" ) );
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
