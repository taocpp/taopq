// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#include "../macros.hpp"

#include <tao/postgres/connection.hpp>
#include <tao/postgres/table_writer.hpp>
#include <tao/utility/getenv.hpp>

void run()
{
   const auto connection = tao::postgres::connection::create( tao::utility::getenv( "TAO_TEST_DATABASE", "dbname=template1" ) );
   connection->execute( "DROP TABLE IF EXISTS tao_table_writer_test" );
   connection->execute( "CREATE TABLE tao_table_writer_test ( a INTEGER NOT NULL, b DOUBLE PRECISION, c TEXT )" );

   std::string buffer = "    10\t1.23\tUSD\n"
                        "    11\t2.34\tCND\n"
                        "    12\t3.45\tEUR\n"
                        "    13\t1.23\tUSD\n"
                        "    14\t1.23\tUSD\n"
                        "    15\t2.34\tCND\n"
                        "    16\t0\t\\N\n    "
                        "    17\t\\N\t\\N\n   "
                        "    18\t2.34\tCND\n"
                        "    19\t1.23\tUSD\n";

   tao::postgres::table_writer tw( connection->direct(), "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
   for( unsigned n = 0; n < 10000; ++n ) {
      for( unsigned i = 0; i < 10; ++i ) {
         buffer[ i * 16 + 0 ] = '0' + ( n / 10000 ) % 10;
         buffer[ i * 16 + 1 ] = '0' + ( n / 1000 ) % 10;
         buffer[ i * 16 + 2 ] = '0' + ( n / 100 ) % 10;
         buffer[ i * 16 + 3 ] = '0' + ( n / 10 ) % 10;
         buffer[ i * 16 + 4 ] = '0' + ( n / 1 ) % 10;
      }
      tw.insert( buffer );
   }

   TEST_ASSERT_MESSAGE( "validate reported result size", tw.finish() == 100000 );
   TEST_ASSERT_MESSAGE( "validate actual result size", connection->execute( "SELECT COUNT(*) FROM tao_table_writer_test" ).as< std::size_t >() == 100000 );

   TEST_THROWS( tao::postgres::table_writer( connection->direct(), "SELECT 42" ) );
   TEST_THROWS( connection->execute( "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" ) );

   TEST_THROWS_MESSAGE( "mixed usage test #1", {
      const auto tr = connection->direct();
      tao::postgres::table_writer tw2( tr, "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
      tr->execute( "SELECT 42" );
      tw2.insert( "1\t0\tXXX\n" );
   } );

   TEST_THROWS_MESSAGE( "mixed usage test #2", {
      const auto tr = connection->direct();
      tao::postgres::table_writer tw2( tr, "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
      tr->execute( "SELECT 42" );
      tw2.finish();
   } );

   connection->execute( "DROP TABLE IF EXISTS tao_table_writer_test" );
   connection->execute( "CREATE TABLE tao_table_writer_test ( a INTEGER NOT NULL, b DOUBLE PRECISION, c TEXT )" );
   {
      tao::postgres::table_writer tw2( connection->direct(), "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
      tw2.insert( "1\t0\tXXX\n" );
      tw2.finish();
   }
   TEST_ASSERT( connection->execute( "SELECT COUNT(*) FROM tao_table_writer_test" ).as< std::size_t >() == 1 );
   {
      tao::postgres::table_writer tw2( connection->direct(), "COPY tao_table_writer_test ( a, b, c ) FROM STDIN" );
      tw2.insert( "2\t0\tXXX\n" );
   }
   TEST_ASSERT( connection->execute( "SELECT COUNT(*) FROM tao_table_writer_test" ).as< std::size_t >() == 1 );

   connection->execute( "DROP TABLE IF EXISTS tao_table_writer_test" );
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
