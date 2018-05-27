// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#include "../macros.hpp"

#include <tao/postgres/connection.hpp>
#include <tao/utility/getenv.hpp>

void check_nested( const std::shared_ptr< tao::postgres::connection >& connection, const std::shared_ptr< tao::postgres::transaction >& tr )
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
      TEST_EXECUTE( tr->subtransaction() );
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
   TEST_EXECUTE( connection->direct() );
   TEST_EXECUTE( connection->transaction() );
}

void run()
{
   const auto connection = tao::postgres::connection::create( tao::utility::getenv( "TAO_TEST_DATABASE", "dbname=template1" ) );

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

   TEST_THROWS( const auto tr = connection->transaction(); connection->transaction() );
   TEST_THROWS( const auto tr = connection->transaction(); connection->direct() );
   TEST_THROWS( const auto tr = connection->direct(); connection->transaction() );
   TEST_THROWS( const auto tr = connection->direct(); connection->direct() );

   TEST_THROWS( const auto tr = connection->transaction(); const auto st = tr->subtransaction(); tr->subtransaction() );
   TEST_THROWS( const auto tr = connection->direct(); const auto st = tr->subtransaction(); tr->subtransaction() );

   TEST_EXECUTE( connection->direct() );
   TEST_EXECUTE( connection->direct()->commit() );
   TEST_EXECUTE( connection->direct()->rollback() );

   TEST_EXECUTE( connection->direct()->subtransaction() );
   TEST_EXECUTE( connection->direct()->subtransaction()->commit() );
   TEST_EXECUTE( connection->direct()->subtransaction()->rollback() );

   TEST_EXECUTE( connection->direct()->subtransaction()->subtransaction() );
   TEST_EXECUTE( connection->direct()->subtransaction()->subtransaction()->commit() );
   TEST_EXECUTE( connection->direct()->subtransaction()->subtransaction()->rollback() );

   TEST_EXECUTE( connection->transaction() );
   TEST_EXECUTE( connection->transaction()->commit() );
   TEST_EXECUTE( connection->transaction()->rollback() );

   TEST_EXECUTE( connection->transaction()->subtransaction() );
   TEST_EXECUTE( connection->transaction()->subtransaction()->commit() );
   TEST_EXECUTE( connection->transaction()->subtransaction()->rollback() );

   TEST_EXECUTE( connection->transaction()->subtransaction()->subtransaction() );
   TEST_EXECUTE( connection->transaction()->subtransaction()->subtransaction()->commit() );
   TEST_EXECUTE( connection->transaction()->subtransaction()->subtransaction()->rollback() );

   TEST_EXECUTE( connection->transaction( tao::postgres::transaction::isolation_level::SERIALIZABLE ) );
   TEST_EXECUTE( connection->transaction( tao::postgres::transaction::isolation_level::REPEATABLE_READ ) );
   TEST_EXECUTE( connection->transaction( tao::postgres::transaction::isolation_level::READ_COMMITTED ) );
   TEST_EXECUTE( connection->transaction( tao::postgres::transaction::isolation_level::READ_UNCOMMITTED ) );

   TEST_EXECUTE( check_nested( connection, connection->direct() ) );
   TEST_EXECUTE( check_nested( connection, connection->transaction() ) );
}

int main()
{
   try {
      run();
   }
   catch( const std::exception& e ) {
      std::cerr << "exception: " << e.what() << std::endl;
      throw;
   }
   catch( ... ) {
      std::cerr << "unknown exception" << std::endl;
      throw;
   }
}
