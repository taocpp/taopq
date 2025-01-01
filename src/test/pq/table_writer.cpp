// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>
#include <tuple>

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq.hpp>

namespace
{
   void run()
   {
      const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );
      connection->execute( "DROP TABLE IF EXISTS tao_table_writer_test" );
      connection->execute( "CREATE TABLE tao_table_writer_test ( a INTEGER NOT NULL, b DOUBLE PRECISION, c TEXT )" );

      tao::pq::table_writer tw( connection->direct(), "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
      TEST_THROWS( connection->direct() );
      for( unsigned n = 0; n < 100000; ++n ) {
         tw.insert( n, n + 23.45, "EUR" );
      }

      tw.insert( std::make_tuple( 123456, tao::pq::null, "EUR\nUSD\"FOO\\BAR" ) );

      TEST_ASSERT_MESSAGE( "validate reported result size", tw.commit() == 100001 );
      TEST_ASSERT_MESSAGE( "validate actual result size", connection->execute( "SELECT COUNT(*) FROM tao_table_writer_test" ).as< std::size_t >() == 100001 );

      {
         const auto [ a, b, c ] = connection->execute( "SELECT a, b, c FROM tao_table_writer_test WHERE b IS NULL" ).tuple< unsigned, std::optional< double >, std::optional< std::string > >();

         TEST_ASSERT_MESSAGE( "checking 'a' value", a == 123456 );
         TEST_ASSERT_MESSAGE( "checking 'b' value", !b );
         TEST_ASSERT_MESSAGE( "checking 'c' value", c == "EUR\nUSD\"FOO\\BAR" );
      }

      TEST_THROWS( tao::pq::table_writer( connection->direct(), "SELECT 42" ) );
      TEST_THROWS( tao::pq::table_writer( connection->direct(), "" ) );
      TEST_THROWS( tao::pq::table_writer( connection->direct(), "COPY tao_table_writer_test ( a, b, c, d ) FROM STDIN" ) );
      TEST_THROWS( tao::pq::table_writer( connection->direct(), "COPY tao_table_writer_test ( a, b, c ) TO STDOUT" ) );

      TEST_THROWS( connection->execute( "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" ) );

      TEST_THROWS_MESSAGE( "mixed usage test #1", {
         const auto tr = connection->direct();
         const tao::pq::table_writer tw2( tr, "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
         tr->execute( "SELECT 42" );
      } );

      TEST_THROWS_MESSAGE( "mixed usage test #2", {
         const auto tr = connection->transaction();
         const tao::pq::table_writer tw2( tr, "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
         tr->execute( "SELECT 42" );
      } );

      connection->execute( "DROP TABLE tao_table_writer_test" );
      connection->execute( "CREATE TABLE tao_table_writer_test ( a INTEGER NOT NULL, b DOUBLE PRECISION, c TEXT )" );
      {
         tao::pq::table_writer tw2( connection->direct(), "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
         tw2.insert_raw( "1\t0\tXXX\n" );
         tw2.commit();
      }
      TEST_ASSERT( connection->execute( "SELECT COUNT(*) FROM tao_table_writer_test" ).as< std::size_t >() == 1 );
      {
         tao::pq::table_writer tw2( connection->direct(), "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
         tw2.insert_raw( "2\t0\tXXX\n" );
      }
      TEST_ASSERT( connection->execute( "SELECT COUNT(*) FROM tao_table_writer_test" ).as< std::size_t >() == 1 );

      connection->execute( "DROP TABLE tao_table_writer_test" );
      connection->execute( "CREATE TABLE tao_table_writer_test ( a INTEGER NOT NULL, b DOUBLE PRECISION, c TEXT )" );
      {
         tao::pq::table_writer tw2( connection->direct(), "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
         tw2.insert_raw( "3\t0\tXXX\n" );
         PQclear( PQexec( connection->underlying_raw_ptr(), "SELECT 42" ) );
         TEST_THROWS( tw2.commit() );
      }
      {
         tao::pq::table_writer tw2( connection->direct(), "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
         tw2.insert_raw( "4\t0\tXXX\n" );
         PQclear( PQexec( connection->underlying_raw_ptr(), "SELECT 42" ) );
         TEST_THROWS( tw2.insert_raw( "5\t0\tXXX\n" ) );
      }

      connection->execute( "DROP TABLE tao_table_writer_test" );
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
