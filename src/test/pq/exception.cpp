// Copyright (c) 2021-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>

void run()
{
   // overwrite the default with an environment variable if needed
   const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );
   const auto connection = tao::pq::connection::create( connection_string );
   connection->execute( "DROP TABLE IF EXISTS tao_exception_test" );

   TEST_THROWS( connection->execute( "SELECT a FROM tao_exception_test" ) );

   connection->execute( "CREATE TABLE tao_exception_test ( a TEXT PRIMARY KEY, b TEXT NOT NULL )" );

   TEST_THROWS( connection->execute( "SELECT c FROM tao_exception_test" ) );

   TEST_THROWS( connection->execute( "FOO BAR BAZ" ) );
   TEST_THROWS( connection->execute( "SELECT 1/0" ) );
   TEST_THROWS( connection->execute( "SELECT * FROM tao_exception_test WHERE a = 42" ) );
   TEST_THROWS( connection->execute( "SELECT * FROM tao_exception_test WHERE a[0] = 'FOO'" ) );
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
