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

   void run()
   {
      // overwrite the default with an environment variable if needed
      const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );

      // open a connection to the database
      const auto conn = tao::pq::connection::create( connection_string );

      // attach a log receiver
      const auto log = std::make_shared< tao::pq::log >();

      log->connection.send_query = []( tao::pq::connection& c, const char* s, int n, const Oid types[], const char* const values[], const int lengths[], const int formats[] ) {
         std::ignore = types;
         std::ignore = lengths;
         std::ignore = formats;
         std::cout << std::format( "send_query(connection={}, statement={}, n_params={})", static_cast< const void* >( &c ), s, n ) << '\n';
         for( int i = 0; i != n; ++i ) {
            std::cout << std::format( "  parameter[{}]={}", i, values[ i ] ) << '\n';
         }
      };
      log->connection.send_query.result = []( tao::pq::connection& c, int r ) {
         std::cout << std::format( "send_query(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
      };
      log->connection.send_query_prepared = []( tao::pq::connection& c, const char* s, int n, const char* const values[], const int lengths[], const int formats[] ) {
         std::ignore = lengths;
         std::ignore = formats;
         std::cout << std::format( "send_query_prepared(connection={}, statement={}, n_params={})", static_cast< const void* >( &c ), s, n ) << '\n';
         for( int i = 0; i != n; ++i ) {
            std::cout << std::format( "  parameter[{}]={}", i, values[ i ] ) << '\n';
         }
      };
      log->connection.send_query_prepared.result = []( tao::pq::connection& c, int r ) {
         std::cout << std::format( "send_query_prepared(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
      };
      log->connection.wait = []( tao::pq::connection& c, bool w, std::chrono::steady_clock::time_point e ) {
         std::cout << std::format( "wait(connection={}, wait_for_write={}, timeout={} ms)", static_cast< const void* >( &c ), w, to_millis( e ) ) << '\n';
      };
      log->connection.poll = []( tao::pq::connection& c, int s, bool w, int t ) {
         std::cout << std::format( "poll(connection={},socket={}, wait_for_write={}, timeout={} ms)", static_cast< const void* >( &c ), s, w, t ) << '\n';
      };
      log->connection.poll.result = []( tao::pq::connection& c, int s, tao::pq::poll::status r ) {
         std::cout << std::format( "poll(connection={},socket={}) -> {}", static_cast< const void* >( &c ), s, r ) << '\n';
      };
      log->connection.is_busy.result = []( const tao::pq::connection& c, int r ) {
         std::cout << std::format( "is_busy(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
      };
      log->connection.consume_input = []( tao::pq::connection& c ) {
         std::cout << std::format( "consume_input(connection={})", static_cast< const void* >( &c ) ) << '\n';
      };
      log->connection.consume_input.result = []( tao::pq::connection& c, int r ) {
         std::cout << std::format( "consume_input(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
      };
      log->connection.flush = []( tao::pq::connection& c ) {
         std::cout << std::format( "flush(connection={})", static_cast< const void* >( &c ) ) << '\n';
      };
      log->connection.flush.result = []( tao::pq::connection& c, int r ) {
         std::cout << std::format( "flush(connection={}) -> {}", static_cast< const void* >( &c ), r ) << '\n';
      };
      log->connection.get_result = []( tao::pq::connection& c, std::chrono::steady_clock::time_point e ) {
         std::cout << std::format( "get_result(connection={}, timeout={} ms)", static_cast< const void* >( &c ), to_millis( e ) ) << '\n';
      };
      log->connection.get_result.result = []( tao::pq::connection& c, PGresult* r ) {
         std::cout << std::format( "get_result(connection={}) -> {}", static_cast< const void* >( &c ), static_cast< const void* >( r ) ) << '\n';
         if( r != nullptr ) {
            const auto st = PQresultStatus( r );
            std::cout << std::format( "  status={}", PQresStatus( st ) ) << '\n';
            const auto cols = PQnfields( r );
            if( cols != 0 ) {
               std::cout << std::format( "  columns={}, rows={}", cols, PQntuples( r ) ) << '\n';
            }
            else {
               const char* str = PQcmdTuples( r );
               if( str[ 0 ] != '\0' ) {
                  std::cout << std::format( "  rows_affected={}", str ) << '\n';
               }
            }
         }
      };

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
