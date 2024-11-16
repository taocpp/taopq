// Copyright (c) 2023-2024 Daniel Frey and Dr. Colin Hirsch
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
      conn->execute( "DROP TABLE IF EXISTS tao_parameter_test" );
      conn->execute( "CREATE TABLE tao_parameter_test ( name TEXT PRIMARY KEY, age INTEGER NOT NULL )" );

      // prepare statements
      conn->prepare( "insert_user", "INSERT INTO tao_parameter_test ( name, age ) VALUES ( $1, $2 )" );

      {
         // begin transaction
         const auto tr = conn->transaction();

         // execute previously prepared statements
         {
            tao::pq::parameter p( "Daniel", 42 );
            tr->execute( "insert_user", p );
            p.reset();
            p.bind( "Tom" );
            tao::pq::parameter p2( 41 );
            tr->execute( "insert_user", tao::pq::parameter( p, p2 ) );
         }

         {
            tao::pq::parameter< 2 > p;
            std::string s = "Alice";
            p.bind( s );
            p.bind( std::move( s ) );
            p.reset( std::string( "Jerry" ), 42 + 7 );
            p.bind();
            tao::pq::parameter<> p2( p );
            tr->execute( "insert_user", p2 );
         }

         {
            tao::pq::parameter< 1 > p;
            std::string s = "Alice";
            p.bind( std::move( s ) );
            tr->execute( "insert_user", p, 44 );
         }

         // commit transaction
         tr->commit();
      }

      // query data
      const auto users = conn->execute( "SELECT name, age FROM tao_parameter_test WHERE age >= $1", 40 );

      // iterate and convert results
      for( const auto& row : users ) {
         std::cout << row[ "name" ].as< std::string >() << " is "
                   << row[ "age" ].as< unsigned >() << " years old.\n";
      }

      {
         tao::pq::parameter< 1 > p;
         p.bind( 1 );
         TEST_THROWS( p.bind( 2 ) );
      }

      {
         tao::pq::parameter< 1 > p;
         tao::pq::parameter< 1 > p2;
         p.bind( 1 );
         p2.bind( 1 );
         TEST_THROWS( p.bind( p2 ) );
      }
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
