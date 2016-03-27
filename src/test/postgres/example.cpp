// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#include <tao/postgres.hpp>

#include <cassert>
#include <iostream>

int main()
{
  const auto connection = tao::postgres::connection::create( "dbname=template1" );

  connection->execute( "DROP TABLE IF EXISTS tao_example" );
  connection->execute( "CREATE TABLE tao_example ( a INTEGER PRIMARY KEY, b INTEGER, c TEXT NOT NULL )" );

  // preparing a statement is optional, but often recommended
  connection->prepare( "insert", "INSERT INTO tao_example VALUES ( $1, $2, $3 )" );

  // use a transaction if needed
  {
    const auto transaction = connection->transaction();
    // execute prepared statements with parameters
    transaction->execute( "insert", 1, 42, "foo" );
    transaction->execute( "insert", 2, tao::postgres::null, "Hello, world!" );
    transaction->commit();
  }

  // insert/update/delete statements return a result which can be queried for the rows affected
  {
    const auto r = connection->execute( "insert", 3, 3, "drei" );
    assert( r.rows_affected() == 1 );
  }

  // queries have a result as well, it contains the returned data
  const auto result = connection->execute( "SELECT * FROM tao_example WHERE b IS NOT NULL" );
  assert( result.size() == 2 );

  // iterate over a result
  for( const auto& row : result ) {
    // access fields by index or (less efficiently) by name
    std::cout << row[ 0 ].as< int >() << ": " << row[ "c" ].as< std::string >() << std::endl;
  }
}
