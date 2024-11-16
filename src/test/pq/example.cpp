// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <iostream>
#include <tao/pq.hpp>

namespace
{
   void run()
   {
      // overwrite the default with an environment variable if needed
      const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );

      // open a connection to the database
      const auto conn = tao::pq::connection::create( connection_string );

      // execute statements
      conn->execute( "DROP TABLE IF EXISTS tao_example" );
      conn->execute( "CREATE TABLE tao_example ( name TEXT PRIMARY KEY, age INTEGER NOT NULL )" );

      // prepare statements
      conn->prepare( "insert_user", "INSERT INTO tao_example ( name, age ) VALUES ( $1, $2 )" );

      {
         // begin transaction
         const auto tr = conn->transaction();

         // execute previously prepared statements
         tr->execute( "insert_user", "Daniel", 42 );
         tr->execute( "insert_user", "Tom", 41 );
         tr->execute( "insert_user", "Jerry", 29 );

         // commit transaction
         tr->commit();
      }

      // query data
      const auto users = conn->execute( "SELECT name, age FROM tao_example WHERE age >= $1", 40 );

      // iterate and convert results
      for( const auto& row : users ) {
         std::cout << row[ "name" ].as< std::string >() << " is "
                   << row[ "age" ].as< unsigned >() << " years old.\n";
      }
   }

}  // namespace

auto main() -> int
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
