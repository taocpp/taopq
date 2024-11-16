// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>
#include <tao/pq/result_traits_optional.hpp>
#include <tao/pq/result_traits_pair.hpp>
#include <tao/pq/result_traits_tuple.hpp>

namespace
{
   void run()
   {
      const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );

      TEST_ASSERT( connection->execute( "SELECT 42" )[ 0 ].as< int >() == 42 );
      TEST_ASSERT( connection->execute( "SELECT 1764" ).at( 0 ).optional< int >() == 1764 );
      TEST_ASSERT( !connection->execute( "SELECT NULL" )[ 0 ].as< std::optional< int > >() );

      TEST_ASSERT( connection->execute( "SELECT 42" )[ 0 ][ 0 ].get() == std::string( "42" ) );
      TEST_ASSERT( connection->execute( "SELECT 42" )[ 0 ][ 0 ].as< int >() == 42 );
      TEST_ASSERT( connection->execute( "SELECT 42" )[ 0 ][ 0 ].optional< int >() == 42 );

      TEST_ASSERT( connection->execute( "SELECT 1, 2" )[ 0 ].pair< int, int >() == std::pair< int, int >( 1, 2 ) );
      TEST_ASSERT( connection->execute( "SELECT 1, 2, 3, 4" )[ 0 ].tuple< int, int, int, int >() == std::tuple< int, int, int, int >( 1, 2, 3, 4 ) );

      TEST_THROWS( connection->execute( "SELECT 42" )[ 0 ].as< bool >() );

      const auto result = connection->execute( "SELECT 1 AS a, 2 AS B, 3 AS \"C\", 4 as \"A\"" );
      const auto& row = result[ 0 ];

      TEST_ASSERT( row.columns() == 4 );

      TEST_ASSERT( row.name( 0 ) == "a" );
      TEST_ASSERT( row.name( 1 ) == "b" );
      TEST_ASSERT( row.name( 2 ) == "C" );
      TEST_ASSERT( row.name( 3 ) == "A" );
      TEST_THROWS( row.name( 4 ) );

      TEST_ASSERT( row.at( 0 ).name() == "a" );
      TEST_ASSERT( row[ 1 ].name() == "b" );
      TEST_ASSERT( row.at( 2 ).name() == "C" );
      TEST_ASSERT( row[ 3 ].name() == "A" );
      TEST_THROWS( row.at( 4 ) );

      TEST_ASSERT( row.index( "a" ) == 0 );
      TEST_ASSERT( row.index( "A" ) == 0 );
      TEST_ASSERT( row.index( "\"a\"" ) == 0 );
      TEST_ASSERT( row.index( "\"A\"" ) == 3 );

      TEST_ASSERT( row.index( "b" ) == 1 );
      TEST_ASSERT( row.index( "B" ) == 1 );
      TEST_ASSERT( row.index( "\"b\"" ) == 1 );
      TEST_THROWS( row.index( "\"B\"" ) );

      TEST_THROWS( row.index( "c" ) );
      TEST_THROWS( row.index( "C" ) );
      TEST_THROWS( row.index( "\"c\"" ) );
      TEST_ASSERT( row.index( "\"C\"" ) == 2 );

      TEST_THROWS( row.get< std::string >( 4 ) );
      TEST_THROWS( row.get< std::optional< std::string > >( 4 ) );
      TEST_THROWS( row.get< std::pair< std::string, std::string > >( 3 ) );

      const auto result2 = connection->execute( "SELECT 1 AS a, 2 AS b, 3 AS a" );
      const auto& row2 = result2[ 0 ];

      TEST_ASSERT( row2.index( "a" ) == 0 );
      TEST_ASSERT( row2.index( "A" ) == 0 );
      TEST_ASSERT( row2.slice( 1, 2 ).index( "a" ) == 1 );
      TEST_ASSERT( row2.slice( 1, 2 ).index( "A" ) == 1 );

      TEST_THROWS( row2.slice( 1, 1 ).index( "a" ) );
      TEST_THROWS( row2.slice( 1, 1 ).index( "A" ) );
      TEST_THROWS( row2.slice( 2, 1 ).index( "b" ) );
      TEST_THROWS( row2.slice( 2, 1 ).index( "B" ) );

      TEST_THROWS( row2.slice( 0, 0 ) );
      TEST_THROWS( row2.slice( 1, 0 ) );
      TEST_THROWS( row2.slice( 2, 0 ) );

      TEST_THROWS( row2.slice( 0, 4 ) );
      TEST_THROWS( row2.slice( 1, 3 ) );
      TEST_THROWS( row2.slice( 2, 2 ) );
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
