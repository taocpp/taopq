// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

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
