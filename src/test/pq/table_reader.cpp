// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq.hpp>

void run()
{
   const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );
   connection->execute( "DROP TABLE IF EXISTS tao_table_reader_test" );
   connection->execute( "CREATE TABLE tao_table_reader_test ( a INTEGER NOT NULL, b DOUBLE PRECISION, c TEXT )" );

   // we use a table_writer to fill the table with 100.000 rows.
   {
      tao::pq::table_writer tw( connection->direct(), "COPY tao_table_reader_test ( a, b, c ) FROM STDIN" );
      for( unsigned n = 0; n < 100000; ++n ) {
         tw.insert( n, n / 100.0, "EUR" );
      }
      TEST_ASSERT_MESSAGE( "validate reported result size", tw.commit() == 100000 );
      TEST_ASSERT_MESSAGE( "validate actual result size", connection->execute( "SELECT COUNT(*) FROM tao_table_reader_test" ).as< std::size_t >() == 100000 );
   }

   {
      tao::pq::table_reader tr( connection->direct(), "COPY tao_table_reader_test ( a, b, c ) TO STDOUT" );
      TEST_THROWS( connection->direct() );

      std::size_t count = 0;
      for( const auto& row : tr ) {
         for( const auto& field : row ) {
            if( !field.is_null() ) {
               ++count;
            }
         }
      }
      TEST_ASSERT_MESSAGE( "validate count", count == 300000 );
   }

   TEST_THROWS( tao::pq::table_reader( connection->direct(), "SELECT 42" ) );
   TEST_THROWS( tao::pq::table_reader( connection->direct(), "" ) );
   TEST_THROWS( tao::pq::table_reader( connection->direct(), "COPY tao_table_reader_test ( a, b, c, d ) TO STDOUT" ) );
   TEST_THROWS( tao::pq::table_reader( connection->direct(), "COPY tao_table_reader_test ( a, b, c ) FROM STDIN" ) );

   TEST_THROWS( connection->execute( "COPY tao_table_reader_test ( a, b, c ) TO STDOUT" ) );

   connection->execute( "DROP TABLE IF EXISTS tao_table_reader_test" );
   connection->execute( "CREATE TABLE tao_table_reader_test ( a BYTEA )" );
   {
      tao::pq::table_writer tw( connection->direct(), "COPY tao_table_reader_test ( a ) FROM STDIN" );
      tw.insert( tao::pq::to_binary_view( "1" ) );
      tw.insert( tao::pq::binary_view() );
      tw.insert( tao::pq::null );
      tw.insert( tao::pq::to_binary_view( "F\"O\\O" ) );
      tw.insert( tao::pq::to_binary_view( "NU\0LL" ) );
      TEST_ASSERT( tw.commit() == 5 );

      tao::pq::table_reader tr( connection->direct(), "COPY tao_table_reader_test ( a ) TO STDOUT" );
      const auto result = tr.vector< std::optional< tao::pq::binary > >();
      TEST_ASSERT( result.size() == 5 );
      TEST_ASSERT( result[ 0 ] == tao::pq::to_binary_view( "1" ) );
      TEST_ASSERT( result[ 1 ] == tao::pq::binary_view() );
      TEST_ASSERT( !result[ 2 ] );
      TEST_ASSERT( result[ 3 ] == tao::pq::to_binary_view( "F\"O\\O" ) );
      TEST_ASSERT( result[ 4 ] == tao::pq::to_binary_view( "NU\0LL" ) );
   }

   connection->execute( "DROP TABLE IF EXISTS tao_table_reader_test" );
   connection->execute( "CREATE TABLE tao_table_reader_test ( a INTEGER NOT NULL, b DOUBLE PRECISION, c TEXT )" );
   connection->execute( "INSERT INTO tao_table_reader_test VALUES( $1, $2, $3 )", 1, 3.141592, "A\bB\fC\"D'E\n\rF\tGH\vI\\J" );
   connection->execute( "INSERT INTO tao_table_reader_test VALUES( $1, $2, $3 )", 2, tao::pq::null, tao::pq::null );
   connection->execute( "INSERT INTO tao_table_reader_test VALUES( $1, $2, $3 )", 3, 42, "FOO" );

   {
      tao::pq::table_reader tr( connection->direct(), "COPY tao_table_reader_test ( a, b, c ) TO STDOUT" );
      TEST_ASSERT( tr.columns() == 3 );
      {
         TEST_ASSERT( tr.get_row() );
         const auto& row = tr.row();
         auto [ a, b, c ] = row.tuple< int, std::optional< double >, std::optional< std::string_view > >();
         TEST_ASSERT( a == 1 );
         TEST_ASSERT( b == 3.141592 );
         TEST_ASSERT( c == "A\bB\fC\"D'E\n\rF\tGH\vI\\J" );

         TEST_ASSERT( row.at( 0 ).as< int >() == 1 );
         TEST_ASSERT( !row[ 1 ].is_null() );
         TEST_ASSERT( row[ 1 ].get() == std::string_view( "3.141592" ) );
         TEST_ASSERT( row[ 1 ].optional< double >() == 3.141592 );
         TEST_THROWS( row.at( 3 ) );
         TEST_THROWS( row.slice( 0, 0 ) );
         TEST_THROWS( row.slice( 1, 0 ) );
         TEST_THROWS( row.slice( 0, 4 ) );
         TEST_THROWS( row.tuple< int, std::optional< double > >() );
         TEST_THROWS( row.tuple< int, std::optional< double >, std::optional< std::string_view >, std::optional< std::string_view > >() );
      }
      {
         TEST_ASSERT( tr.get_row() );
         auto [ a, b, c ] = tr.row().tuple< int, std::optional< double >, std::optional< std::string_view > >();
         TEST_ASSERT( a == 2 );
         TEST_ASSERT( !b );
         TEST_ASSERT( !c );
         TEST_THROWS( tr.row().tuple< int, double, std::string_view >() );
      }
      PQclear( PQexec( connection->underlying_raw_ptr(), "SELECT 42" ) );
      TEST_THROWS( tr.get_row() );
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
