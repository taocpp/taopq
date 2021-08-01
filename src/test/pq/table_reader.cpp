// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq.hpp>

void run()
{
   const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );
   connection->execute( "DROP TABLE IF EXISTS tao_table_reader_test" );
   connection->execute( "CREATE TABLE tao_table_reader_test ( a INTEGER NOT NULL, b DOUBLE PRECISION, c TEXT )" );

   // we use a table_writer to fill the table with 100.000 rows.
   tao::pq::table_writer tw( connection->direct(), "COPY tao_table_reader_test ( a, b, c ) FROM STDIN" );
   for( unsigned n = 0; n < 100000; ++n ) {
      tw.insert( n, n / 100.0, "EUR" );
   }
   TEST_ASSERT_MESSAGE( "validate reported result size", tw.commit() == 100000 );
   TEST_ASSERT_MESSAGE( "validate actual result size", connection->execute( "SELECT COUNT(*) FROM tao_table_reader_test" ).as< std::size_t >() == 100000 );

   {
      tao::pq::table_reader tr( connection->direct(), "COPY tao_table_reader_test ( a, b, c ) TO STDOUT" );
      TEST_THROWS( connection->direct() );
      std::size_t count = 0;
      while( tr.fetch_next() ) {
         (void)tr.get_data();
         ++count;
      }
      TEST_ASSERT_MESSAGE( "validate count", count == 100000 );
   }

   TEST_THROWS( tao::pq::table_reader( connection->direct(), "SELECT 42" ) );
   TEST_THROWS( tao::pq::table_reader( connection->direct(), "" ) );
   TEST_THROWS( tao::pq::table_reader( connection->direct(), "COPY tao_table_reader_test ( a, b, c, d ) TO STDOUT" ) );
   TEST_THROWS( tao::pq::table_reader( connection->direct(), "COPY tao_table_reader_test ( a, b, c ) FROM STDIN" ) );

   TEST_THROWS( connection->execute( "COPY tao_table_reader_test ( a, b, c ) TO STDOUT" ) );
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
