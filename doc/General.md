# General

**Work in Progress** -- [for now please read this](temporary.md)

* [Connections](#connections)
* [Transactions](#transactions)
* [Statements](#statements)
* [Results](#results)

## Connections

A connection is opened with `tao::pq::connection::create( connect_info )`.
The parameter `connect_info` is a connection string according to the [PostgreSQL documentation](https://www.postgresql.org/docs/current/static/libpq-connect.html#LIBPQ-CONNSTRING).

The return value is a `std::shared_ptr< tao::pq::connection >` which always points to a valid connection; in case of errors an exception is thrown.

For the remainder of the documentation we assume `c` to be a valid `std::shared_ptr< tao::pq::connection >`.

## Transactions

Given a connection, a transaction (with the default [transaction isolation level](Advanced-Features#transaction-isolation) can be obtained by calling `c->transaction()`.

Similarly, a "direct", i.e. "fake" auto-commit transaction that behaves just like executing statements directly on the connection can be obtained by calling `c->direct()`.

The return value in either case is a valid `std::shared_ptr< tao::pq::transaction >`; in case of errors an exception is thrown.

For the remainder of the documentation let `tr` be a `std::shared_ptr< tao::pq::transaction >`.

For the remainder of this section `tr` is assumed to not be a direct transaction.

Transactions can be committed or rolled back via a single use of either `tr->commit()` or `tr->rollback()`.
If a transaction is destroyed before one of these functions is called it is automatically rolled back by the destructor.

Usually a piece of code will create a transaction, execute some SQL statements on it, and then commit it.
Rolling back is frequently left to the transaction destructor in case of exceptions or early returns.

Note that transactions contain a shared pointer to the connection they were created from.
This makes the following code a valid shortcut for when a connection is used for a single transaction.

```c++
const auto tr = tao::pq::connection::create( "connection_string" )->transaction();
// Use transaction normally since the connection is kept alive by the transaction.
```

## Statements

SQL statements are executed on a transaction by calling `foo->execute( statement, parameters... )` where `foo` can be either a transaction or a connection.

```c++
const auto r1 = tr->execute( "SELECT * FROM users" );
const auto r2 = tr->execute( "DELETE FROM users WHERE id = $1", 42 );
```

Parameters to the statement, in particular when variable, [should always be](Overview.md#important) implemented by using placeholders in the statement and passing the values as additional arguments to the function call as it is both safer and more efficient.

SQL `NULL` values can be passed either as `tao::pq::null`.
A parameter of type `std::optional< T >` will behave like `tao::pq::null` or a parameter of type `T` depending on whether the optional has a value.

The result of the `execute()` functions is of type `tao::pq::result` as [introduced below](#results).

### Prepared Statements

Statements can be [prepared](https://www.postgresql.org/docs/current/sql-prepare.html) for greater efficiency.

Prepared statements are per connection and are created with a `c->prepare( name, statement )` function call where `name` can be any C++ identifier.
Statements can only be prepared on a per-connection base.

```c++
c->prepare( "SelectUsers", "SELECT * FROM users" );
c->prepare( "DeleteUser", "DELETE FROM users WHERE id = $1" );
```

Once a statement is prepared it can be executed by using its name instead of the SQL statement in a call to `execute()`.

```c++
const auto r1 = tr->execute( "SelectUsers" );
const auto r2 = tr->execute( "DeleteUser", 42 );
```

A prepared statement can also be removed with a call to `c->deallocate( name )`.

## Results

The return value of `execute()` is of type `tao::pq::result`.

For statements like `UPDATE` or `SELECT` the result contains the number of affected (or returned) rows.
A complete list of statements for which this number is available can be found in the [PostgreSQL documentation](https://www.postgresql.org/docs/11/libpq-exec.html), see `PQcmdTuples()`.

```c++
const auto r2 = tr->execute( "DeleteUser", 42 );
assert( r2.has_affected_rows() );
const std::size_t ar = r2.affected_rows();
```

For all other statements, nothing useful can be done with the result.

For `SELECT` statements the result also contains the data returned by the query, the result set, and its description.

Working with a result set is documented on the [Result Sets](Result-Sets.md) page.

Note that `tao::pq::result` has value semantics and is relatively cheap to copy.

Copyright (c) 2019 Daniel Frey and Dr. Colin Hirsch
