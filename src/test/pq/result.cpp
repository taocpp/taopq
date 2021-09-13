// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>
#include <tao/pq/result_traits_optional.hpp>
#include <tao/pq/result_traits_pair.hpp>
#include <tao/pq/result_traits_tuple.hpp>

void run()  // NOLINT(readability-function-size)
{
   const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );

   TEST_EXECUTE( connection->execute( "SELECT NULL" ) );
   TEST_ASSERT( connection->execute( "SELECT NULL" ).is_null( 0, 0 ) );
   TEST_THROWS( connection->execute( "SELECT NULL" ).is_null( 0, 1 ) );
   TEST_THROWS( connection->execute( "SELECT NULL" ).is_null( 1, 0 ) );
   TEST_THROWS( connection->execute( "SELECT NULL" ).get( 0, 0 ) );
   TEST_THROWS( connection->execute( "SELECT NULL" ).get( 0, 1 ) );
   TEST_THROWS( connection->execute( "SELECT NULL" ).get( 1, 0 ) );
   TEST_ASSERT( connection->execute( "SELECT NULL" )[ 0 ].is_null( 0 ) );
   TEST_THROWS( connection->execute( "SELECT NULL" )[ 0 ].is_null( 1 ) );
   TEST_ASSERT( connection->execute( "SELECT NULL" )[ 0 ][ 0 ].is_null() );
   TEST_ASSERT( connection->execute( "SELECT NULL" )[ 0 ][ 0 ] == tao::pq::null );
   TEST_ASSERT( !( connection->execute( "SELECT NULL" )[ 0 ][ 0 ] != tao::pq::null ) );
   TEST_THROWS( connection->execute( "SELECT NULL" )[ 0 ].at( 1 ) );
   TEST_THROWS( connection->execute( "SELECT NULL" ).at( 1 )[ 1 ] );

   TEST_EXECUTE( connection->execute( "SELECT 42" ) );
   TEST_ASSERT( !connection->execute( "SELECT 42" ).is_null( 0, 0 ) );
   TEST_THROWS( connection->execute( "SELECT 42" ).is_null( 0, 1 ) );
   TEST_THROWS( connection->execute( "SELECT 42" ).is_null( 1, 0 ) );
   TEST_ASSERT( connection->execute( "SELECT 42" ).get( 0, 0 ) == std::string( "42" ) );
   TEST_THROWS( connection->execute( "SELECT 42" ).get( 0, 1 ) );
   TEST_THROWS( connection->execute( "SELECT 42" ).get( 1, 0 ) );
   TEST_ASSERT( !connection->execute( "SELECT 42" )[ 0 ].is_null( 0 ) );
   TEST_THROWS( connection->execute( "SELECT 42" )[ 0 ].is_null( 1 ) );
   TEST_ASSERT( !connection->execute( "SELECT 42" )[ 0 ][ 0 ].is_null() );
   TEST_ASSERT( connection->execute( "SELECT 42" )[ 0 ][ 0 ] != tao::pq::null );
   TEST_ASSERT( !( connection->execute( "SELECT 42" )[ 0 ][ 0 ] == tao::pq::null ) );
   TEST_THROWS( connection->execute( "SELECT 42" )[ 0 ].at( 1 ) );
   TEST_THROWS( connection->execute( "SELECT 42" ).at( 1 )[ 1 ] );

   TEST_ASSERT( connection->execute( "SELECT 42" ).as< int >() == 42 );
   TEST_ASSERT( connection->execute( "SELECT 1764" ).optional< int >() == 1764 );
   TEST_ASSERT( !connection->execute( "SELECT 64 WHERE FALSE" ).optional< int >() );
   TEST_ASSERT( !connection->execute( "SELECT NULL" ).as< std::optional< int > >() );

   TEST_ASSERT( connection->execute( "SELECT 42" ).tuple< int >() == std::tuple< int >( 42 ) );

   TEST_ASSERT( connection->execute( "SELECT 1, 2" ).pair< int, int >() == std::pair< int, int >( 1, 2 ) );
   TEST_ASSERT( connection->execute( "SELECT 1, 2, 3, 4" ).tuple< int, int, int, int >() == std::tuple< int, int, int, int >( 1, 2, 3, 4 ) );

   TEST_ASSERT( connection->execute( "SELECT 42" ).columns() == 1 );
   TEST_ASSERT( connection->execute( "SELECT 42" ).vector< int >().size() == 1 );
   TEST_ASSERT( connection->execute( "SELECT 42" ).list< int >().size() == 1 );
   TEST_ASSERT( connection->execute( "SELECT 42" ).set< int >().size() == 1 );
   TEST_ASSERT( connection->execute( "SELECT 42" ).multiset< int >().size() == 1 );
   TEST_ASSERT( connection->execute( "SELECT 42" ).unordered_set< int >().size() == 1 );
   TEST_ASSERT( connection->execute( "SELECT 42" ).unordered_multiset< int >().size() == 1 );

   TEST_THROWS( connection->execute( "SELECT 42" ).as< bool >() );

   TEST_ASSERT( connection->execute( "SELECT 1, 2" ).columns() == 2 );
   TEST_ASSERT( connection->execute( "SELECT 1, 2" ).map< int, int >().size() == 1 );
   TEST_ASSERT( connection->execute( "SELECT 1, 2" ).multimap< int, int >().size() == 1 );
   TEST_ASSERT( connection->execute( "SELECT 1, 2" ).unordered_map< int, int >().size() == 1 );
   TEST_ASSERT( connection->execute( "SELECT 1, 2" ).unordered_multimap< int, int >().size() == 1 );

   TEST_ASSERT( connection->execute( "SELECT 1 UNION ALL SELECT 2" ).list< int >().size() == 2 );
   TEST_ASSERT( connection->execute( "SELECT 1, 2, 3, 4 UNION ALL SELECT 5, 6, 7, 8" ).list< std::tuple< int, int, int, int > >().size() == 2 );
   TEST_ASSERT( connection->execute( "SELECT 1, 2 UNION ALL SELECT 2, 5 UNION ALL SELECT 3, 42" ).map< int, int >().size() == 3 );

   const auto result = connection->execute( "SELECT 1 AS a, 2 AS B, 3 AS \"C\"" );

   TEST_ASSERT( result.has_rows_affected() );
   TEST_ASSERT( !result.empty() );
   TEST_ASSERT( result.size() == 1 );
   TEST_ASSERT( result.columns() == 3 );
   TEST_ASSERT( result.at( 0 ).columns() == 3 );
   TEST_THROWS( result.at( 1 ) );

   TEST_ASSERT( result.name( 0 ) == "a" );
   TEST_ASSERT( result.name( 1 ) == "b" );
   TEST_ASSERT( result.name( 2 ) == "C" );

   TEST_ASSERT( result.index( "a" ) == 0 );
   TEST_ASSERT( result.index( "A" ) == 0 );
   TEST_ASSERT( result.index( "\"a\"" ) == 0 );
   TEST_THROWS( result.index( "\"A\"" ) );

   TEST_ASSERT( result.index( "b" ) == 1 );
   TEST_ASSERT( result.index( "B" ) == 1 );
   TEST_ASSERT( result.index( "\"b\"" ) == 1 );
   TEST_THROWS( result.index( "\"B\"" ) );

   TEST_THROWS( result.index( "c" ) );
   TEST_THROWS( result.index( "C" ) );
   TEST_THROWS( result.index( "\"c\"" ) );
   TEST_ASSERT( result.index( "\"C\"" ) == 2 );

   TEST_THROWS( connection->execute( "SELECT 42 WHERE FALSE" ).as< int >() );
   TEST_THROWS( connection->execute( "SELECT 1 UNION ALL SELECT 2" ).as< int >() );

   TEST_THROWS( connection->execute( "SELECT 42" ).pair< int, int >() );
   TEST_THROWS( connection->execute( "SELECT 1, 2" ).as< int >() );

   TEST_THROWS( connection->execute( "SELECT ''" ).as< char >() );
   TEST_THROWS( connection->execute( "SELECT 'Hallo'" ).as< char >() );

   TEST_THROWS( connection->execute( "SELECT ''" ).as< signed char >() );
   TEST_THROWS( connection->execute( "SELECT 'Hallo'" ).as< signed char >() );
   TEST_THROWS( connection->execute( "SELECT -129" ).as< signed char >() );
   TEST_ASSERT( connection->execute( "SELECT -128" ).as< signed char >() == -128 );
   TEST_ASSERT( connection->execute( "SELECT 127" ).as< signed char >() == 127 );
   TEST_THROWS( connection->execute( "SELECT 128" ).as< signed char >() );

   TEST_THROWS( connection->execute( "SELECT ''" ).as< unsigned char >() );
   TEST_THROWS( connection->execute( "SELECT 'Hallo'" ).as< unsigned char >() );
   TEST_ASSERT( connection->execute( "SELECT 255" ).as< unsigned char >() == 255 );
   TEST_THROWS( connection->execute( "SELECT 256" ).as< unsigned char >() );

   TEST_THROWS( connection->execute( "SELECT ''" ).as< short >() );
   TEST_THROWS( connection->execute( "SELECT 'Hallo'" ).as< short >() );
   TEST_THROWS( connection->execute( "SELECT -32769" ).as< short >() );
   TEST_ASSERT( connection->execute( "SELECT -32768" ).as< short >() == -32768 );
   TEST_ASSERT( connection->execute( "SELECT 32767" ).as< short >() == 32767 );
   TEST_THROWS( connection->execute( "SELECT 32768" ).as< short >() );

   TEST_THROWS( connection->execute( "SELECT ''" ).as< unsigned short >() );
   TEST_THROWS( connection->execute( "SELECT 'Hallo'" ).as< unsigned short >() );
   TEST_ASSERT( connection->execute( "SELECT 65535" ).as< unsigned short >() == 65535 );
   TEST_THROWS( connection->execute( "SELECT 65536" ).as< unsigned short >() );

   TEST_THROWS( connection->execute( "SELECT ''" ).as< int >() );
   TEST_THROWS( connection->execute( "SELECT 'Hallo'" ).as< int >() );
   TEST_THROWS( connection->execute( "SELECT -2147483649" ).as< int >() );
   TEST_ASSERT( connection->execute( "SELECT -2147483648" ).as< int >() == -2147483648LL );
   TEST_ASSERT( connection->execute( "SELECT 2147483647" ).as< int >() == 2147483647 );
   TEST_THROWS( connection->execute( "SELECT 2147483648" ).as< int >() );

   TEST_THROWS( connection->execute( "SELECT ''" ).as< unsigned >() );
   TEST_THROWS( connection->execute( "SELECT 'Hallo'" ).as< unsigned >() );
   TEST_ASSERT( connection->execute( "SELECT 4294967295" ).as< unsigned >() == 4294967295 );
   TEST_THROWS( connection->execute( "SELECT 4294967296" ).as< unsigned >() );

   TEST_THROWS( connection->execute( "SELECT '42 FOO'" ).as< unsigned >() );
   TEST_THROWS( connection->execute( "SELECT '42BAR'" ).as< unsigned >() );

   int count = 0;
   for( const auto& row : connection->execute( "SELECT 1 UNION ALL SELECT 2" ) ) {
      for( const auto& field : row ) {
         TEST_ASSERT( field.as< int >() == ++count );
      }
   }
   TEST_ASSERT( count == 2 );
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
