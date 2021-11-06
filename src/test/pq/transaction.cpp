// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tuple>

#include <tao/pq/connection.hpp>

template< typename Connection, typename Transaction >
void check_nested( const std::shared_ptr< Connection >& connection, const std::shared_ptr< Transaction >& tr )
{
   TEST_THROWS( connection->direct() );
   TEST_THROWS( connection->transaction() );
   TEST_EXECUTE( tr->execute( "SELECT 42" ) );
   {
      const auto tr2 = tr->subtransaction();
      TEST_THROWS( tr->subtransaction() );
      TEST_EXECUTE( tr2->execute( "SELECT 42" ) );
      {
         const auto tr3 = tr2->subtransaction();
         TEST_THROWS( tr2->subtransaction() );
         TEST_EXECUTE( tr3->execute( "SELECT 42" ) );
         TEST_THROWS( tr2->execute( "SELECT 42" ) );
         TEST_EXECUTE( tr3->commit() );
         TEST_THROWS( tr3->execute( "SELECT 42" ) );
      }
      TEST_THROWS( tr->execute( "SELECT 42" ) );
      TEST_EXECUTE( tr2->commit() );
      TEST_THROWS( tr2->execute( "SELECT 42" ) );
      TEST_THROWS( tr2->subtransaction() );
      TEST_EXECUTE( std::ignore = tr->subtransaction() );
   }
   tr->execute( "SELECT 42" );
   {
      const auto tr2 = tr->subtransaction();
      TEST_EXECUTE( tr2->execute( "SELECT 42" ) );
      TEST_THROWS( tr->execute( "SELECT 42" ) );
      TEST_EXECUTE( tr2->rollback() );
      TEST_THROWS( tr2->execute( "SELECT 42" ) );
   }
   TEST_EXECUTE( tr->execute( "SELECT 42" ) );
   TEST_EXECUTE( tr->commit() );
   TEST_THROWS( tr->execute( "SELECT 42" ) );
   TEST_EXECUTE( std::ignore = connection->direct() );
   TEST_EXECUTE( std::ignore = connection->transaction() );
}

void run()
{
   const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );

   connection->execute( "DROP TABLE IF EXISTS tao_transaction_test" );
   connection->execute( "CREATE TABLE tao_transaction_test ( a INTEGER PRIMARY KEY )" );

   TEST_ASSERT( connection->execute( "SELECT * FROM tao_transaction_test" ).empty() );

   TEST_EXECUTE( connection->execute( "INSERT INTO tao_transaction_test VALUES ( 1 )" ) );  // auto-commit
   TEST_ASSERT( connection->execute( "SELECT * FROM tao_transaction_test" ).size() == 1 );

   TEST_EXECUTE( connection->direct()->execute( "INSERT INTO tao_transaction_test VALUES ( 2 )" ) );  // auto-commit
   TEST_ASSERT( connection->execute( "SELECT * FROM tao_transaction_test" ).size() == 2 );

   TEST_EXECUTE( connection->transaction()->execute( "INSERT INTO tao_transaction_test VALUES ( 3 )" ) );  // not committed
   TEST_ASSERT( connection->execute( "SELECT * FROM tao_transaction_test" ).size() == 2 );

   TEST_EXECUTE( connection->direct()->subtransaction()->execute( "INSERT INTO tao_transaction_test VALUES ( 3 )" ) );  // not committed
   TEST_ASSERT( connection->execute( "SELECT * FROM tao_transaction_test" ).size() == 2 );

   TEST_EXECUTE( connection->transaction()->subtransaction()->execute( "INSERT INTO tao_transaction_test VALUES ( 3 )" ) );  // not committed
   TEST_ASSERT( connection->execute( "SELECT * FROM tao_transaction_test" ).size() == 2 );

   TEST_THROWS( connection->transaction( tao::pq::access_mode::read_only )->execute( "INSERT INTO tao_transaction_test VALUES ( 3 )" ) );
   TEST_ASSERT( connection->transaction( tao::pq::access_mode::read_only )->execute( "SELECT * FROM tao_transaction_test" ).size() == 2 );

   TEST_THROWS_MESSAGE( "THROWS connection->transaction()", const auto tr = connection->transaction(); std::ignore = connection->transaction() );
   TEST_THROWS_MESSAGE( "THROWS connection->direct()", const auto tr = connection->transaction(); std::ignore = connection->direct() );
   TEST_THROWS_MESSAGE( "THROWS connection->transaction()", const auto tr = connection->direct(); std::ignore = connection->transaction() );
   TEST_THROWS_MESSAGE( "THROWS connection->direct()", const auto tr = connection->direct(); std::ignore = connection->direct() );

   TEST_THROWS_MESSAGE( "THROWS tr->subtransaction()", const auto tr = connection->transaction(); const auto st = tr->subtransaction(); std::ignore = tr->subtransaction() );
   TEST_THROWS_MESSAGE( "THROWS tr->subtransaction()", const auto tr = connection->direct(); const auto st = tr->subtransaction(); std::ignore = tr->subtransaction() );

   TEST_EXECUTE( std::ignore = connection->direct() );
   TEST_EXECUTE( connection->direct()->commit() );
   TEST_EXECUTE( connection->direct()->rollback() );

   TEST_EXECUTE( std::ignore = connection->direct()->subtransaction() );
   TEST_EXECUTE( connection->direct()->subtransaction()->commit() );
   TEST_EXECUTE( connection->direct()->subtransaction()->rollback() );

   TEST_EXECUTE( std::ignore = connection->direct()->subtransaction()->subtransaction() );
   TEST_EXECUTE( connection->direct()->subtransaction()->subtransaction()->commit() );
   TEST_EXECUTE( connection->direct()->subtransaction()->subtransaction()->rollback() );

   TEST_EXECUTE( std::ignore = connection->transaction() );
   TEST_EXECUTE( connection->transaction()->commit() );
   TEST_EXECUTE( connection->transaction()->rollback() );

   TEST_EXECUTE( std::ignore = connection->transaction()->subtransaction() );
   TEST_EXECUTE( connection->transaction()->subtransaction()->commit() );
   TEST_EXECUTE( connection->transaction()->subtransaction()->rollback() );

   TEST_EXECUTE( std::ignore = connection->transaction()->subtransaction()->subtransaction() );
   TEST_EXECUTE( connection->transaction()->subtransaction()->subtransaction()->commit() );
   TEST_EXECUTE( connection->transaction()->subtransaction()->subtransaction()->rollback() );

   TEST_EXECUTE( std::ignore = connection->transaction( tao::pq::isolation_level::serializable ) );
   TEST_EXECUTE( std::ignore = connection->transaction( tao::pq::isolation_level::repeatable_read ) );
   TEST_EXECUTE( std::ignore = connection->transaction( tao::pq::isolation_level::read_committed ) );
   TEST_EXECUTE( std::ignore = connection->transaction( tao::pq::isolation_level::read_uncommitted ) );

   TEST_EXECUTE( std::ignore = connection->transaction( tao::pq::access_mode::read_write ) );
   TEST_EXECUTE( std::ignore = connection->transaction( tao::pq::access_mode::read_only ) );

   TEST_EXECUTE( check_nested( connection, connection->direct() ) );
   TEST_EXECUTE( check_nested( connection, connection->transaction() ) );
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
