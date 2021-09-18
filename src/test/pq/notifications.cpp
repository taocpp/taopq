// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>

std::size_t counter = 0;

void handle_notification( const tao::pq::notification& n )
{
   std::cout << n.channel_name() << " received " << n.payload() << std::endl;
   ++counter;
}

void run()
{
   // overwrite the default with an environment variable if needed
   const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );
   const auto connection = tao::pq::connection::create( connection_string );

   TEST_EXECUTE( connection->set_notification_handler( handle_notification ) );
   TEST_EXECUTE( connection->listen( "FOO" ) );
   TEST_ASSERT( counter == 0 );

   TEST_EXECUTE( connection->notify( "FOO" ) );
   TEST_ASSERT( counter == 1 );

   TEST_EXECUTE( connection->notify( "FOO", "with payload" ) );
   TEST_ASSERT( counter == 2 );

   TEST_EXECUTE( connection->unlisten( "FOO" ) );
   TEST_EXECUTE( connection->notify( "FOO" ) );
   TEST_EXECUTE( connection->get_notifications() );
   TEST_ASSERT( counter == 2 );

   TEST_ASSERT( connection->notification_handler() );
   TEST_EXECUTE( connection->reset_notification_handler() );
   TEST_ASSERT( !connection->notification_handler() );
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
