// Copyright (c) 2021-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "utils/macros.hpp"

#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include <tao/pq/internal/resize_uninitialized.hpp>

namespace
{
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

      test( s, 2 );
      TEST_ASSERT( s[ 0 ] == 'h' );
      TEST_ASSERT( s[ 1 ] == 'e' );

      std::vector< std::byte > v = { static_cast< std::byte >( 42 ), static_cast< std::byte >( 69 ) };
      TEST_ASSERT( v.size() == 2 );
      TEST_ASSERT( v[ 0 ] == static_cast< std::byte >( 42 ) );
      TEST_ASSERT( v[ 1 ] == static_cast< std::byte >( 69 ) );

      tao::pq::internal::resize_uninitialized( v, 5 );
      TEST_ASSERT( v.size() == 5 );
      TEST_ASSERT( v[ 0 ] == static_cast< std::byte >( 42 ) );
      TEST_ASSERT( v[ 1 ] == static_cast< std::byte >( 69 ) );

      tao::pq::internal::resize_uninitialized( v, 1000000000 );
      TEST_ASSERT( v.size() == 1000000000 );
      TEST_ASSERT( v[ 0 ] == static_cast< std::byte >( 42 ) );
      TEST_ASSERT( v[ 1 ] == static_cast< std::byte >( 69 ) );

      tao::pq::internal::resize_uninitialized( v, 2 );
      TEST_ASSERT( v.size() == 2 );
      TEST_ASSERT( v[ 0 ] == static_cast< std::byte >( 42 ) );
      TEST_ASSERT( v[ 1 ] == static_cast< std::byte >( 69 ) );
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
