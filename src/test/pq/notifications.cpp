// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection.hpp>

#if defined( _WIN32 )
#include <winsock.h>
#else
#include <unistd.h>
#endif

std::size_t counter = 0;

void handle_notification( const tao::pq::notification& n )
{
   std::cout << "channel '" << n.channel() << "' received '" << n.payload() << "'\n";
   ++counter;
}

std::size_t foo_counter = 0;

void handle_foo_notification( const char* payload )
{
   std::cout << "foo handler received '" << payload << "'\n";
   ++foo_counter;
}

void run()
{
   // overwrite the default with an environment variable if needed
   const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );
   const auto connection = tao::pq::connection::create( connection_string );

   TEST_EXECUTE( connection->set_notification_handler( handle_notification ) );
   TEST_EXECUTE( connection->listen( "FOO", handle_foo_notification ) );
   TEST_ASSERT( counter == 0 );
   TEST_ASSERT( foo_counter == 0 );

   TEST_EXECUTE( connection->notify( "FOO" ) );
   TEST_ASSERT( counter == 1 );
   TEST_ASSERT( foo_counter == 1 );

   TEST_ASSERT( connection->notification_handler( "FOO" ) );
   TEST_ASSERT( !connection->notification_handler( "BAR" ) );
   TEST_EXECUTE( connection->reset_notification_handler( "FOO" ) );
   TEST_ASSERT( !connection->notification_handler( "FOO" ) );

   TEST_EXECUTE( connection->notify( "FOO", "with payload" ) );
   TEST_ASSERT( counter == 2 );
   TEST_ASSERT( foo_counter == 1 );

   TEST_EXECUTE( connection->unlisten( "FOO" ) );
   TEST_EXECUTE( connection->notify( "FOO" ) );
   TEST_EXECUTE( connection->get_notifications() );
   TEST_ASSERT( counter == 2 );

   TEST_ASSERT( connection->notification_handler() );
   TEST_EXECUTE( connection->reset_notification_handler() );
   TEST_ASSERT( !connection->notification_handler() );

#if defined( _WIN32 )
   closesocket( PQsocket( connection->underlying_raw_ptr() ) );
#else
   close( PQsocket( connection->underlying_raw_ptr() ) );
#endif

   TEST_THROWS( connection->get_notifications() );
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
