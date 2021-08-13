// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include "../getenv.hpp"
#include "../macros.hpp"

#include <limits>

#include <tao/pq/connection.hpp>
#include <tao/pq/large_object.hpp>

template< typename T >
void test( const std::shared_ptr< tao::pq::connection >& connection, const std::basic_string< T >& data )
{
   const auto transaction = connection->transaction();
   const auto oid = tao::pq::large_object::create( transaction );
   tao::pq::large_object lo( transaction, oid, std::ios_base::in | std::ios_base::out );
   lo.write( data );
   lo.seek( 0, std::ios_base::beg );
   const auto result = lo.read< std::basic_string< T > >( 10 );  // by default returns 'binary'
   TEST_ASSERT( result.size() == 5 );
   TEST_ASSERT( result == data );
   TEST_THROWS( lo.resize( -5 ) );
}

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

   test< char >( connection, "hello" );
   test< unsigned char >( connection, reinterpret_cast< const unsigned char* >( "world" ) );
   test< std::byte >( connection, reinterpret_cast< const std::byte* >( "nice!" ) );

   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      tao::pq::large_object lo( transaction, oid, std::ios_base::in );
      TEST_THROWS( lo.write( "abc" ) );
   }

   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      tao::pq::large_object lo( transaction, oid, std::ios_base::out );
      TEST_THROWS( lo.read( std::numeric_limits< unsigned >::max() ) );
   }

   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      tao::pq::large_object lo( transaction, oid, std::ios_base::in | std::ios_base::out );
      lo.close();
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

   {
      const auto transaction = connection->transaction();
      const auto oid = tao::pq::large_object::create( transaction );
      const char* filename = "dummy.txt";
      tao::pq::large_object::export_file( transaction, oid, filename );
      const auto oid2 = tao::pq::large_object::import_file( transaction, filename );
      TEST_ASSERT( oid != oid2 );
      TEST_THROWS( tao::pq::large_object::export_file( transaction, oid, "" ) );
   }

   {
      const auto transaction = connection->transaction();
      TEST_THROWS( tao::pq::large_object::import_file( transaction, "" ) );
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
