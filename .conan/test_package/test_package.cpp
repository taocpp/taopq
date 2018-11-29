// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#include <cstdlib>
#include <iostream>

#include <tao/pq.hpp>

#include "../../src/test/getenv.hpp"

int main()
{
   try {
      const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );
      connection->execute( "DROP TABLE IF EXISTS taopq_conan_test" );
      connection->execute( "CREATE TABLE taopq_conan_test ( a INTEGER PRIMARY KEY )" );
   }
   catch( const std::runtime_error& error ) {
      // ignore connection fault
      std::cerr << error.what() << std::endl;
   }

   return EXIT_SUCCESS;
}
