// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <exception>
#include <iostream>

#include <tao/pq.hpp>
#include <tao/pq/internal/unreachable.hpp>

namespace
{
   void run()
   {
      // overwrite the default with an environment variable if needed
      const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );  // NOLINT(clang-analyzer-deadcode.DeadStores)

      // open a connection
      const auto connection = tao::pq::connection::create( connection_string );
      TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::off );

      TEST_THROWS( connection->pipeline_sync() );

      connection->exit_pipeline_mode();
      TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::off );

      connection->enter_pipeline_mode();
      TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::on );

      connection->exit_pipeline_mode();
      TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::off );

      {
         auto tr = connection->direct();
         TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::off );

         connection->enter_pipeline_mode();
         TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::on );

         tr->send( "SELECT 42" );
         tr->send( "SELECT 1234" );
         connection->pipeline_sync();

         TEST_ASSERT( tr->get_result().as< int >() == 42 );

         tr->send( "SELECT 1701" );
         connection->pipeline_sync();

         TEST_ASSERT( tr->get_result().as< int >() == 1234 );
         tr->consume_pipeline_sync();

         TEST_ASSERT( tr->get_result().as< int >() == 1701 );
         tr->consume_pipeline_sync();

         TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::on );
         connection->exit_pipeline_mode();
         TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::off );

         tr->commit();
      }

      {
         auto tr = connection->transaction();
         TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::off );

         connection->enter_pipeline_mode();
         TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::on );

         tr->send( "SELECT 42" );
         tr->send( "SELECT 1234" );
         connection->pipeline_sync();

         TEST_ASSERT( tr->get_result().as< int >() == 42 );

         tr->send( "SELECT 1701" );
         connection->pipeline_sync();

         TEST_ASSERT( tr->get_result().as< int >() == 1234 );
         tr->consume_pipeline_sync();

         TEST_ASSERT( tr->get_result().as< int >() == 1701 );
         tr->consume_pipeline_sync();

         TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::on );
         connection->exit_pipeline_mode();
         TEST_ASSERT( connection->pipeline_status() == tao::pq::pipeline_status::off );

         tr->commit();
      }
   }

}  // namespace

auto main() -> int  // NOLINT(bugprone-exception-escape)
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
