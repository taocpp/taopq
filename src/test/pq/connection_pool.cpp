// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection_pool.hpp>

void run()
{
   // overwrite the default with an environment variable if needed
   const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );

   const auto pool = tao::pq::connection_pool::create( connection_string );

   TEST_ASSERT( pool->connection() );
   TEST_ASSERT( pool->connection()->execute( "SELECT 1" ).as< int >() == 1 );

   const auto conn = pool->connection();
   TEST_ASSERT( pool->connection() );
   TEST_ASSERT( conn->execute( "SELECT 2" ).as< int >() == 2 );

   const auto pool2 = tao::pq::connection_pool::create( connection_string );
   TEST_ASSERT( pool->connection()->execute( "SELECT 3" ).as< int >() == 3 );
   TEST_ASSERT( pool2->connection()->execute( "SELECT 4" ).as< int >() == 4 );
   TEST_ASSERT( conn->execute( "SELECT 5" ).as< int >() == 5 );
   TEST_ASSERT( pool2->connection()->execute( "SELECT 6" ).as< int >() == 6 );
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
