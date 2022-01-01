// Copyright (c) 2021-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../macros.hpp"

#include <tao/pq/internal/resize_uninitialized.hpp>

void test( std::string& s, const std::size_t size )
{
   tao::pq::internal::resize_uninitialized( s, size );
   TEST_ASSERT( s.size() == size );
   TEST_ASSERT( s[ size ] == '\0' );
}

void run()
{
   std::string s = "hello";
   test( s, 2 );
   TEST_ASSERT( s[ 0 ] == 'h' );
   TEST_ASSERT( s[ 1 ] == 'e' );

   test( s, 5 );
   TEST_ASSERT( s[ 0 ] == 'h' );
   TEST_ASSERT( s[ 1 ] == 'e' );

   test( s, 32 );
   TEST_ASSERT( s[ 0 ] == 'h' );
   TEST_ASSERT( s[ 1 ] == 'e' );

   test( s, 1000000000 );
   TEST_ASSERT( s[ 0 ] == 'h' );
   TEST_ASSERT( s[ 1 ] == 'e' );
}

auto main() -> int
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
