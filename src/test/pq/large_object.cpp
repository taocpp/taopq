// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>
#include <tao/pq/large_object.hpp>

void run()
{
   const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );

   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      tao::pq::large_object lo( transaction, oid, std::ios_base::in | std::ios_base::out );
      tao::pq::large_object lo2 = std::move( lo );
      lo = std::move( lo2 );
      TEST_ASSERT( lo.tell() == 0 );
      lo.resize( 42 );
      lo.seek( 20, std::ios_base::beg );
      TEST_ASSERT( lo.tell() == 20 );
      lo.seek( -20, std::ios_base::end );
      TEST_ASSERT( lo.tell() == 22 );
      lo.seek( 5, std::ios_base::cur );
      TEST_ASSERT( lo.tell() == 27 );
      lo.seek( -7, std::ios_base::cur );
      TEST_ASSERT( lo.tell() == 20 );
      TEST_THROWS( lo.seek( -60, std::ios_base::end ) );
   }
   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      tao::pq::large_object lo( transaction, oid, std::ios_base::in | std::ios_base::out );
      TEST_THROWS( lo.resize( -5 ) );
   }
   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      tao::pq::large_object lo( transaction, oid, std::ios_base::in | std::ios_base::out );
      tao::pq::large_object::remove( transaction, oid );
      TEST_THROWS( lo.tell() );
   }
   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      TEST_THROWS( tao::pq::large_object::create( transaction, oid ) );
   }
   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      tao::pq::large_object::remove( transaction, oid );
      TEST_THROWS( tao::pq::large_object::remove( transaction, oid ) );
   }
   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      tao::pq::large_object::remove( transaction, oid );
      TEST_THROWS( tao::pq::large_object( transaction, oid, std::ios_base::in | std::ios_base::out ) );
   }
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
