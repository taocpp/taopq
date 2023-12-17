// Copyright (c) 2023 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <iostream>
#include <tao/pq.hpp>

void run()
{
   // overwrite the default with an environment variable if needed
   const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );

   // open a connection to the database
   const auto conn = tao::pq::connection::create( connection_string );

   // execute statements
   conn->execute( "DROP TABLE IF EXISTS tao_parameter" );
   conn->execute( "CREATE TABLE tao_parameter ( name TEXT PRIMARY KEY, age INTEGER NOT NULL )" );

   // prepare statements
   conn->prepare( "insert_user", "INSERT INTO tao_parameter ( name, age ) VALUES ( $1, $2 )" );

   {
      // begin transaction
      const auto tr = conn->transaction();

      // execute previously prepared statements
      {
         tao::pq::parameter p( "Daniel", 42 );
         tr->execute( "insert_user", p );
         p.reset();
         p.bind( "Tom" );
         p.bind( 41 );
         tr->execute( "insert_user", p );
      }

      {
         tao::pq::parameter< 2 > p;
         std::string s = "Jerry";
         p.bind( s );
         p.reset( std::string( "Jerry" ), 42 + 7 );
         p.bind();
         tr->execute( "insert_user", p );
      }

      // commit transaction
      tr->commit();
   }

   // query data
   const auto users = conn->execute( "SELECT name, age FROM tao_parameter WHERE age >= $1", 40 );

   // iterate and convert results
   for( const auto& row : users ) {
      std::cout << row[ "name" ].as< std::string >() << " is "
                << row[ "age" ].as< unsigned >() << " years old.\n";
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
