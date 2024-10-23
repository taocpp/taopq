// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <limits>

#include <tao/pq/binary.hpp>
#include <tao/pq/connection.hpp>
#include <tao/pq/large_object.hpp>

template< typename R, typename T >
void test( const std::shared_ptr< tao::pq::connection >& connection, const T& data )
{
   const auto transaction = connection->transaction();

   const auto oid = tao::pq::large_object::create( transaction );
   tao::pq::large_object lo( transaction, oid, std::ios_base::in | std::ios_base::out );
   lo.write( data );

   lo.seek( 0, std::ios_base::beg );

   const auto result = lo.read< R >( 10 );
   TEST_ASSERT( result.size() == 5 );
   TEST_ASSERT( result[ 0 ] == data[ 0 ] );
   TEST_ASSERT( result[ 1 ] == data[ 1 ] );
   TEST_ASSERT( result[ 2 ] == data[ 2 ] );
   TEST_ASSERT( result[ 3 ] == data[ 3 ] );
   TEST_ASSERT( result[ 4 ] == data[ 4 ] );
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
      TEST_ASSERT( lo.seek( 20, std::ios_base::beg ) == 20 );
      TEST_ASSERT( lo.tell() == 20 );
      TEST_ASSERT( lo.seek( -20, std::ios_base::end ) == 22 );
      TEST_ASSERT( lo.tell() == 22 );
      TEST_ASSERT( lo.seek( 5, std::ios_base::cur ) == 27 );
      TEST_ASSERT( lo.tell() == 27 );
      TEST_ASSERT( lo.seek( -7, std::ios_base::cur ) == 20 );
      TEST_ASSERT( lo.tell() == 20 );
      TEST_THROWS( lo.seek( -60, std::ios_base::end ) );
   }

   test< std::string >( connection, std::string( "hello" ) );
   test< std::string >( connection, std::string_view( "hello" ) );

   test< tao::pq::binary >( connection, tao::pq::to_binary( "nice!" ) );
   test< tao::pq::binary >( connection, tao::pq::to_binary_view( "nice!" ) );

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
