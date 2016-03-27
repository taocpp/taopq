// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#include "../macros.hpp"

#include <tao/utility/getenv.hpp>
#include <tao/postgres/connection.hpp>
#include <tao/postgres/result_traits_optional.hpp>
#include <tao/postgres/result_traits_pair.hpp>
#include <tao/postgres/result_traits_tuple.hpp>

int main()
{
  const auto connection = tao::postgres::connection::create( tao::utility::getenv( "TEST_DATABASE", "dbname=template1" ) );

  TEST_ASSERT( connection->execute( "SELECT 42" ).as< int >() == 42 );
  TEST_ASSERT( connection->execute( "SELECT 1764" ).optional< int >() == 1764 );
  TEST_ASSERT( !connection->execute( "SELECT 64 WHERE FALSE" ).optional< int >() );
  TEST_ASSERT( !connection->execute( "SELECT NULL" ).as< tao::optional< int > >() );

  TEST_ASSERT( connection->execute( "SELECT $1::INTEGER", tao::optional< int >( 42 ) ).as< tao::optional< int > >() == 42 );
  TEST_ASSERT( !connection->execute( "SELECT $1::INTEGER", tao::optional< int >() ).as< tao::optional< int > >() );

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

  const auto result = connection->execute( "SELECT 1 AS a, 2 AS B, 3 AS \"C\"");

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

  int count = 0;
  for( const auto& row : connection->execute( "SELECT 1 UNION ALL SELECT 2" ) ) {
    TEST_ASSERT( row.as< int >() == ++count );
  }
  TEST_ASSERT( count == 2 );
}
