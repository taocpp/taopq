// Copyright (c) 2020-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <exception>
#include <iostream>
#include <string>

#include <tao/pq.hpp>

namespace example
{
   struct user
   {
      std::string name;
      int age;
      std::string planet;
   };

}  // namespace example

template<>
inline constexpr bool tao::pq::is_aggregate< example::user > = true;

namespace
{
   void run()
   {
      const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );

      connection->execute( "DROP TABLE IF EXISTS tao_aggregate_test" );
      connection->execute( "CREATE TABLE tao_aggregate_test ( name TEXT PRIMARY KEY, age INTEGER, planet TEXT )" );

      {
         const example::user u{
            .name = "R. Giskard Reventlov",
            .age = 42,
            .planet = "Aurora"
         };
         TEST_EXECUTE( connection->execute( "INSERT INTO tao_aggregate_test VALUES ( $1, $2, $3 )", u ) );
      }

      for( const example::user u : connection->execute( "SELECT name, age, planet FROM tao_aggregate_test" ) ) {  // NOLINT(performance-for-range-copy)
         TEST_ASSERT( u.name == "R. Giskard Reventlov" );
         TEST_ASSERT( u.age == 42 );
         TEST_ASSERT( u.planet == "Aurora" );
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
