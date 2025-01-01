// Copyright (c) 2024-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <exception>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>

#include <tao/pq.hpp>

namespace
{
   [[nodiscard]] auto to_millis( std::chrono::steady_clock::time_point end ) noexcept
   {
      return std::chrono::duration_cast< std::chrono::milliseconds >( end - std::chrono::steady_clock::now() ).count();
   }

   void log_send_query( tao::pq::connection& c, const char* s, int n, const Oid types[], const char* const values[], const int lengths[], const int formats[] )
   {
      std::ignore = types;
      std::ignore = lengths;
      std::ignore = formats;
      std::cout << std::format( "send_query(connection={}, statement={}, n_params={})", static_cast< const void* >( &c ), s, n ) << '\n';
      for( int i = 0; i != n; ++i ) {
         std::cout << std::format( "  parameter[{}]={})", i, values[ i ] ) << '\n';
      }
   }

   void log_send_query_result( tao::pq::connection& c, int r )
   {
      std::cout << std::format( "send_query(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
   }

   void log_send_query_prepared( tao::pq::connection& c, const char* s, int n, const char* const values[], const int lengths[], const int formats[] )
   {
      std::ignore = lengths;
      std::ignore = formats;
      std::cout << std::format( "send_query_prepared(connection={}, statement={}, n_params={})", static_cast< const void* >( &c ), s, n ) << '\n';
      for( int i = 0; i != n; ++i ) {
         std::cout << std::format( "  parameter[{}]={})", i, values[ i ] ) << '\n';
      }
   }

   void log_send_query_prepared_result( tao::pq::connection& c, int r )
   {
      std::cout << std::format( "send_query_prepared(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
   }

   void log_wait( tao::pq::connection& c, bool w, std::chrono::steady_clock::time_point e )
   {
      std::cout << std::format( "wait(connection={}, wait_for_write={}, timeout={}ms)", static_cast< const void* >( &c ), w, to_millis( e ) ) << '\n';
   }

   void log_poll( tao::pq::connection& c, int s, bool w, int t )
   {
      std::cout << std::format( "poll(connection={},socket={}, wait_for_write={}, timeout={}ms)", static_cast< const void* >( &c ), s, w, t ) << '\n';
   }

   void log_poll_result( tao::pq::connection& c, int s, tao::pq::poll::status r )
   {
      std::cout << std::format( "poll(connection={},socket={}) -> {}", static_cast< const void* >( &c ), s, r ) << '\n';
   }

   void log_is_busy_result( const tao::pq::connection& c, int r )
   {
      std::cout << std::format( "is_busy(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
   }

   void log_consume_input( tao::pq::connection& c )
   {
      std::cout << std::format( "consume_input(connection={})", static_cast< const void* >( &c ) ) << '\n';
   }

   void log_consume_input_result( tao::pq::connection& c, int r )
   {
      std::cout << std::format( "consume_input(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
   }

   void log_flush( tao::pq::connection& c )
   {
      std::cout << std::format( "flush(connection={})", static_cast< const void* >( &c ) ) << '\n';
   }

   void log_flush_result( tao::pq::connection& c, int r )
   {
      std::cout << std::format( "flush(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
   }

   void log_get_result( tao::pq::connection& c, std::chrono::steady_clock::time_point e )
   {
      std::cout << std::format( "get_result(connection={}, timeout={}ms)", static_cast< const void* >( &c ), to_millis( e ) ) << '\n';
   }

   void log_get_result_result( tao::pq::connection& c, PGresult* r )
   {
      std::cout << std::format( "get_result(connection={}) -> {}", static_cast< const void* >( &c ), static_cast< const void* >( r ) ) << '\n';
   }

   void run()
   {
      // overwrite the default with an environment variable if needed
      const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );

      // open a connection to the database
      const auto conn = tao::pq::connection::create( connection_string );

      // attach a log receiver
      const auto log = std::make_shared< tao::pq::log >();

      log->connection.send_query = log_send_query;
      log->connection.send_query.result = log_send_query_result;
      log->connection.send_query_prepared = log_send_query_prepared;
      log->connection.send_query_prepared.result = log_send_query_prepared_result;
      log->connection.wait = log_wait;
      log->connection.poll = log_poll;
      log->connection.poll.result = log_poll_result;
      log->connection.is_busy.result = log_is_busy_result;
      log->connection.consume_input = log_consume_input;
      log->connection.consume_input.result = log_consume_input_result;
      log->connection.flush = log_flush;
      log->connection.flush.result = log_flush_result;
      log->connection.get_result = log_get_result;
      log->connection.get_result.result = log_get_result_result;
      conn->set_log_handler( log );

      // execute statements
      conn->execute( "DROP TABLE IF EXISTS tao_example" );
      conn->execute( "CREATE TABLE tao_example ( name TEXT PRIMARY KEY, age INTEGER NOT NULL )" );

      // prepare statements
      conn->prepare( "insert_user", "INSERT INTO tao_example ( name, age ) VALUES ( $1, $2 )" );

      {
         // begin transaction
         const auto tr = conn->transaction();

         // execute previously prepared statements
         tr->execute( "insert_user", "Daniel", 42 );
         tr->execute( "insert_user", "Tom", 41 );
         tr->execute( "insert_user", "Jerry", 29 );

         // commit transaction
         tr->commit();
      }

      // query data
      const auto users = conn->execute( "SELECT name, age FROM tao_example WHERE age >= $1", 40 );

      // iterate and convert results
      for( const auto& row : users ) {
         std::cout << row[ "name" ].as< std::string >() << " is "
                   << row[ "age" ].as< unsigned >() << " years old.\n";
      }
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
