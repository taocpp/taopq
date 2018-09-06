// Copyright (c) 2014-2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/postgres/

#include <cstdlib>
#include <iostream>

#include <tao/postgres.hpp>
#include <tao/utility/getenv.hpp>

int main()
{
    try {
        const auto connection = tao::postgres::connection::create( tao::utility::getenv( "TAO_TEST_DATABASE", "dbname=template1" ) );
        connection->execute( "DROP TABLE IF EXISTS tao_transaction_test" );
        connection->execute( "CREATE TABLE tao_transaction_test ( a INTEGER PRIMARY KEY )" );
    } catch (const std::runtime_error& error) {
        // ignore connection fault
        std::cerr << error.what() << std::endl;
    }

   return EXIT_SUCCESS;
}
