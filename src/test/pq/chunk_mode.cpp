// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <exception>
#include <iostream>
#include <string>

#include <tao/pq.hpp>

namespace
{
   void run()
   {
      // overwrite the default with an environment variable if needed
      const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );

      // open a connection to the database
      const auto conn = tao::pq::connection::create( connection_string );

      conn->execute( "DROP TABLE IF EXISTS tao_single_row_mode" );
      conn->execute( "CREATE TABLE tao_single_row_mode ( name TEXT PRIMARY KEY, age INTEGER NOT NULL )" );

      conn->prepare( "insert_user", "INSERT INTO tao_single_row_mode ( name, age ) VALUES ( $1, $2 )" );
      conn->execute( "insert_user", "Daniel", 42 );
      conn->execute( "insert_user", "Tom", 41 );
      conn->execute( "insert_user", "Jerry", 29 );
      conn->execute( "insert_user", "Alice", 32 );
      conn->execute( "insert_user", "Bob", 19 );
      conn->execute( "insert_user", "Charlie", 45 );

      const auto tr = conn->transaction();
      tr->send( "SELECT name, age FROM tao_single_row_mode" );
      tr->set_chunk_mode( 2 );

      while( true ) {
         const auto result = tr->get_result();
         if( result.empty() ) {
            break;
         }

         for( const auto& row : result ) {
            std::cout << row[ "name" ].as< std::string >() << " is "
                      << row[ "age" ].as< unsigned >() << " years old.\n";
         }
      }

      TEST_THROWS( tr->set_single_row_mode() );
      TEST_THROWS( tr->set_chunk_mode( 2 ) );

      tr->send( "SELECT name, age FROM tao_single_row_mode" );
      TEST_THROWS( tr->set_chunk_mode( 0 ) );
      TEST_THROWS( tr->set_chunk_mode( -1 ) );
      tr->set_chunk_mode( 2 );
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
