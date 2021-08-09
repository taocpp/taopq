// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>
#include <tao/pq/large_object.hpp>

void run()
{
   const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );
   const auto transaction = connection->transaction();

   const auto oid = tao::pq::large_object::create( transaction );
   {
      tao::pq::large_object lo( transaction, oid, std::ios_base::in | std::ios_base::out );
      TEST_ASSERT( lo.tell() == 0 );
   }
   tao::pq::large_object::remove( transaction, oid );
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
