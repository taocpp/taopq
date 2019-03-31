# taoPQ Documentation

## Table of content

 * [Project](https://github.com/taocpp/taopq)
 * [General](#general)
 * [Connections](#connections)
 * [Transactions](#transactions)
 * [Executing Statements](#executing-statements)
 * [Preparing Statements](#preparing-statements)
 * [Results](#results)
 * [Result Sets](#result-sets)
 * [Rows](#rows)
 * [Fields](#fields)
 * [Custom Data Types for Parameters](#custom-data-types-for-parameters)
 * [Custom Data Types for Result Sets](#custom-data-types-for-result-sets)
 * [Connection Pools](#connection-pools)
 * [Requirements](#requirements)
 * [Installation](#installation)

## Overview

You start by opening a connection, either directly or you create a connection pool to provide connections on demand.

From a connection you open a transaction, several sub-types of transactions (including nested transactions) are available.

On a transaction you can then execute statements.
You can choose between prepared statements or direct statements.

Parameters are passed as additional parameters to the function call, no manual string manipulation/escaping is necessary.
Parameters can be of almost any type, several types are predefined by the library.
Custom data types can be registered via traits.

You can use `std::optional` to model the NULL value of SQL, giving you seemless and intuitive support for NULL-able values.
You can also use `tao::pq::null` as a parameter if you want to pass NULL explicitly.

Each statement then returns a result, which either contains the number of rows affected in case of `INSERT`, `UPDATE`, `DELETE`, ... statements, or it contains a result set if the statement was a `SELECT` statement.

If the result contains a result set, you can either iterate directly or have it conveniently converted into a C++ data structure.
STL containers, `std::pair`/`std::tuple`, `std::optional` for NULL-able values and custom data types registered via traits are supported.

Some convenience methods are provided to keep simple use-cases simple, e.g. if you have a connection pool you can get an (implicit) transaction, execute a statement, and convert the result to an STL container with just a single, simple line of code which hides the intermediate steps.

## Connections

A connection is opened with `tao::pq::connection::create( connect_info )`.
The parameter `connect_info` is a connection string according to the [PostgreSQL documentation](https://www.postgresql.org/docs/current/static/libpq-connect.html#LIBPQ-CONNSTRING).

The return value is a `std::shared_ptr< tao::pq::connection >` which is guaranteed to be valid; in case of errors an exception is thrown.

For the remainder of this documentation we assume `c` to be a valid `std::shared_ptr< tao::pq::connection >`.

## Transactions

Given a connection, you can either get a "direct" transaction by calling `c->direct()`, or you get a regular transaction by calling `c->transaction()`.
Like connections, transactions are also handled via `std::shared_ptr<>`.
For the remainder of the documentation, let `tr` be a `std::shared_ptr< tao::pq::transaction >`.
On all transactions you can also open a (regular) sub-transaction, including nested sub-transactions, by calling `tr->subtransaction()`.

Direct transactions are pseudo-transaction where statements on the connection are executed in autocommit-mode.

Regular top-level transactions can also be passed one of the isolation levels from `tao::pq::transaction::isolation_level` by calling `c->transaction( level )`.

```c++
enum class isolation_level
{
   default_isolation_level,
   serializable,
   repeatable_read,
   read_committed,
   read_uncommitted
};
```

Regular transactions need to be committed (or rolled back), call `tr->commit()` or `tr->rollback()` explicitly.
If a transaction is not explicitly committed or rolled back, the destructor will implicitly roll back the transaction.
This is helpful in the presence of C++ exceptions.
You usually execute SQL statements and other C++ code and, at the end of your transaction call `tr->commit()`.
If an exception is thrown, the transaction is automatically rolled back.
Note that this also works for sub-transactions, only the statements executed in the sub-transaction are rolled back.

The library also ensures that the logical order of statements executed within transaction as well as opening and committing, rolling back and destructing transactions makes sense.
If you open two regular transactions (on the same connection) and execute statements on both without committing (or rolling back or destroying) the other transaction first, the library will notice and throw an appropriate `std::logic_error`.

## Executing Statements

You can execute SQL statements on a transaction by calling `tr->execute( statement, parameters... )`.
This is best shown by some simple examples:

```c++
const auto r1 = tr->execute( "SELECT * FROM users" );
const auto r2 = tr->execute( "DELETE FROM users WHERE id = $1", 42 );
```

As you can see from the second example, all values should be passed as C++ parameters and the statement itself should only contain placeholders.
This avoids SQL injection attacks and it is also more efficient.

If you need to set a value to SQL's `NULL`, you can pass `tao::pq::null`.
Custom data types can be registered via traits, documentation will follow later.
For now, just have a look at the examples in the repository.

For a more convenient use you can also execute statements on a connection, which will implicitly create a (temporary) direct transaction.

## Preparing Statements

You can prepare statements for more efficient execution.
Statements can only be prepared on a per-connection base, hence you can prepare a statement with `c->prepare( name, statement )`.
Name can be any valid C++ identifier.
Example:

```c++
c->prepare( "SelectUsers", "SELECT * FROM users" );
c->prepare( "DeleteUser", "DELETE FROM users WHERE id = $1" );
```

Once statements are prepared, you can execute them by their name:

```c++
const auto r1 = tr->execute( "SelectUsers" );
const auto r2 = tr->execute( "DeleteUser", 42 );
```

There is no special syntax to distinguish prepared statements from direct SQL statements.
Each connection simply remembers the names of all prepared statements.
If you ever need to unprepare a prepared statement, use `c->deallocate( name )`.

## Results

Results are values, i.e. they are not handled via `std::shared_ptr<>`.
They are copyable, but not assignable or movable.
For the remainder of the documentation, let `rs` be a `tao::pq::result`.

If a result does *not* contain a result set, i.e. if it is not the result of a `SELECT` statement, you can query the number of rows affected by the statements by calling `rs.rows_affected()`.
Statements that do not return a number of affected rows and that also do not return a result set, e.g. `CREATE TABLE`, will return 0.
You can query if a result has only the number of affected rows by calling `rs.has_affected_rows()`.
This will only return `false` if the statement returned a result set.

## Result Sets

Statements that return a result set allow you to query the returned rows as well as some meta-information about the result.

You can query the number of columns by calling `rs.columns()`.
You can get the name or a column by calling `rs.name( index )` (where `index` must be from 0 to less than `rs.columns()`).
You can also get the index of a column by name, by calling `rs.index( name )`.
Note that name matching is done according to the SQL standard.

You can query the number of rows by calling `rs.size()` and, since the result set may also contain no rows, you can call `rs.empty()`.
You can iterate over the rows of a result set by using `rs.begin()` and `rs.end()`, which also allows to iterate over a result set with a range-based for-loop.
Alternatively, you can access rows by their row number by using `rs[ row ]` or `rs.at( row )`.

Lastly, you can access the (raw) data stored at each field with two methods `rs.is_null( row, column )` and `rs.get( row, column )`.
The latter call is only allowed if the former returned `false`.
The `get` methods returns a `const char*`, which is valid for as long as the lifetime of (any copy of) the result.

Using direct access if often not necessary, but it might be helpful for generic function, e.g. for debugging purposes.
Access methods to the data via type traits is documented later, as their understanding depends on the methods provided by rows and fields.

Regular access happens by iterating over the rows of the result set, or by conveniently converting the whole result set into a value or a container.

###### `rs.as< T >()`

If you expect the result set to contain a single row, you may call `rs.as< T >()` as a shortcut for `rs[ 0 ].as< T >()`.
If the result set contain no rows or more than one row, an exception is thrown.

###### `rs.optional< T >()`

If you except a result set with either no rows or a single row, you may call `rs.optional< T >()`.
If the result set is empty, it will return an empty `std::optional< T >`.
Otherwise it will return a `std::optional< T >` initialized with the result of `rs.as< T >()`.

###### `rs.pair< T, U >()`

Short-cut for `rs.as< std::pair< T, U > >()`.

###### `rs.tuple< T, ... >()`

Short-cut for `rs.as< std::tuple< T, ... > >()`.

###### `rs.as_container< T >()`

Default constructs a value `T tmp`, calls `tmp.reserve( rs.size() )` (if possible).
It then calls `tmp.insert( tmp.end(), r.as< typename T::value_type >() )` for each row `r` of the result set.
Finally, `tmp` is returned.

###### `rs.vector< T, ... >()`

Short-cut for `rs.as_container< std::vector< T, ... > >()`.

###### `rs.list< T, ... >()`

Short-cut for `rs.as_container< std::list< T, ... > >()`.

###### `rs.set< T, ... >()`

Short-cut for `rs.as_container< std::set< T, ... > >()`.

###### `rs.multiset< T, ... >()`

Short-cut for `rs.as_container< std::multiset< T, ... > >()`.

###### `rs.unordered_set< T, ... >()`

Short-cut for `rs.as_container< std::unordered_set< T, ... > >()`.

###### `rs.unordered_multiset< T, ... >()`

Short-cut for `rs.as_container< std::unordered_multiset< T, ... > >()`.

###### `rs.map< T, U, ... >()`

Short-cut for `rs.as_container< std::map< T, U, ... > >()`.

###### `rs.multimap< T, U, ... >()`

Short-cut for `rs.as_container< std::multimap< T, U, ... > >()`.

###### `rs.unordered_map< T, U, ... >()`

Short-cut for `rs.as_container< std::unordered_map< T, U, ... > >()`.

###### `rs.unordered_multimap< T, U, ... >()`

Short-cut for `rs.as_container< std::unordered_multimap< T, U, ... > >()`.

## Rows

For the remainder of the documentation, let `r` be a `tao::pq::row`.
Rows are provided by a result set, you can not create them manually.
Rows are valid for as long as the lifetime of (any copy of) the result.

Like with the result set itself, you can query meta-information about the columns.
This information will the the same as for the result set.
You can query the number of columns by calling `r.columns()`.
You can get the name or a column by calling `r.name( index )` (where `index` must be from 0 to less than `r.columns()`).
You can also get the index of a column by name, by calling `r.index( name )`.
Note that name matching is done according to the SQL standard.

You can access a row's fields by their field number by using `r[ index ]` or `r.at( index )`, or you can access them by their field name by using `r[ name ]` or `r.at( name )`.
The latter is less efficient as it will call `r.index( name )` each time.

As with the result set, you can access the (raw) data stored at each field with two methods `r.is_null( column )` and `r.get( column )`.
The latter call is only allowed if the former returned `false`.
The `get` methods returns a `const char*`, which is valid for as long as the lifetime of (any copy of) the result.

Using direct access if often not necessary, but it might be helpful for generic function, e.g. for debugging purposes.
Access methods to the data via type traits is documented later, as their understanding depends on the methods provided by fields.

###### Slicing

From a row `r` you can create a sub-range by calling `r.slice( offset, size )`.
If the sub-range is outside of the available columns of `r`, an exception is thrown.
The returned row will allow zero-based access to only the sub-range of the original row.

###### `r.get< T >( column )`

TODO

###### `r.optional< T >( column )`

Short-cut for `r.get< std::optional< T > >( column )`.

###### `r.as< T >()`

Returns `r.get< T >( 0 )` if the size of the row is suitable for `T`, otherwise an exception is thrown.

###### `r.optional< T >()`

Short-cut for `r.as< std::optional< T > >()`.

###### `r.pair< T, U >()`

Short-cut for `r.as< std::pair< T, U > >()`.

###### `r.tuple< T, ... >()`

Short-cut for `r.as< std::tuple< T, ... > >()`.

## Fields

For the remainder of the documentation, let `f` be a `tao::pq::field`.
Fields are provided by a result set or row, you can not create them manually.
Fields are valid for as long as the lifetime of (any copy of) the result.

As with the result set or rows, you can access the (raw) data stored at a field with two methods `f.is_null()` and `f.get()`.
The latter call is only allowed if the former returned `false`.
The `get` methods returns a `const char*`, which is valid for as long as the lifetime of (any copy of) the result.
Instead of calling `f.is_null()`, you can also compare a field against `tao::pq::null` with `==` or `!=`.

###### `f.as< T >()`

TODO

###### `f.optional< T >()`

Short-cut for `f.as< std::optional< T > >()`.

## Custom Data Types for Parameters

TODO

## Custom Data Types for Result Sets

TODO

## Connection Pools

TODO

## Requirements

TODO

## Installation

TODO

Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch



# Design Overview

The design of taoPQ is loosely based on Jeroen T. Vermeulen's [libpqxx](http://pqxx.org/development/libpqxx/), keeping the philosophy of the design while rethinking everything from the ground up and taking C++17 into account where possible.

The most important classes and design decisions are listed below.

### `tao::pq::connection`

Each connection to a server is handled by an instance of `tao::pq::connection`. A connection is created by a static method `tao::pq::connection::create()`, which returns a `std::shared_ptr` to the connection. From a connection you can get a transaction by calling `direct()` or `transaction()`, where the former is an auto-commit transaction while the latter creates a *real* transaction.

### `tao::pq::transaction`

The base class for all transactions. Like connections, transactions are handled via `std::shared_ptr`. SQL commands are always executed within a transaction with the `execute()` method. You can also get a subtransacion by calling `subtransaction()`. The subtransaction is, again, a transaction and you can create nested subtransactions. The library will ensure that transactions are used with correct nesting, for each connection there is at most one active transaction that can be used. For each transaction you should call either `commit()` or `rollback()`, otherwise the dtor will try to call `rollback()`. After calling either `commit()` or `rollback()`, a transaction is automatically marked inactive. Usage of inactive transactions results in an exception being thrown.

### Execution of SQL statements

Executing an SQL statement is done by calling `execute()` on either a transaction or on a connection directly. Calling it on a connection is equivalent of creating animplicit auto-commit transaction and forwarding the `execute()` call to that transaction. Transaction order/nesting is checked before any statement is executed.

The statement may contain placeholders `$1`, `$2`, ... which are passed as additional parameters to `execute()`. By *not* concatenating parameters into the SQL statement string, you gain two major advantages: It is more efficient and you don't have to worry about SQL injection attacks - the latter is simply not possible when using placeholders.

### Prepared statements

On a connection, you can prepare statements with a symbolic name by calling `prepare()`. Simply pass the symbolic name instead of the actual SQL command to `execute()` to call the prepared statement, including the parameters for the placeholders. A prepared statement can be un-prepared by calling `deallocate()`.

### `tao::pq::result`

Each execution of an SQL command returns a result. A result is a non-modifiable container which contains either the number of rows affected by an INSERT/UPDATE/DELETE statement or a result set which contains any number of rows returned from a SELECT statement. It is usually clear which type of result is returned from a statement, in case of doubt (or for a more generic result handler), you can call `has_rows_affected()`.

### `tao::pq::row`

A single row from a result set. Each row has one or more fields, accessible either by index or by name.

### `tao::pq::field`

A single field from a result set's row. Each field can be converted to different data types.

### SQL `NULL`

SQL `NULL` can be supplied as `tao::pq::null` as a parameter to a statement explicitly or implicitly when passing a `std::optional` as a parameter. Result fields can be queried explicitly by using `is_null()` or implicitly returning `std::optional`s.

### Data Types

A set of predefined data types is available for both parameters (converting a type `T` into a string for PostgreSQL's API) and for results (converting a string returned from the database into a type `T`). The user can specialize both `tao::pq::parameter_traits` as well as `tao::pq::result_traits` to extend the API. Each data type may correspond to one or more result fields.

Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
