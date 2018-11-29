# Design Overview

The design of The Art of C++ / taopq is loosely based on Jeroen T. Vermeulen's [libpqxx](http://pqxx.org/development/libpqxx/), keeping the philosophy of the design while rethinking everything from the ground up and taking C++17 into account where possible.

The most important classes and design decisions are listed below.

### `tao::pq::connection`

Each connection to a server is handled by an instance of `tao::pq::connection`. A connection is created by a static method `tao::pq::connection::create()`, which returns a `std::shared_ptr` to the connection. From a connection instance you can get a transaction by calling `direct()` or `transaction()`, where the former is an auto-commit transaction while the latter creates a *real* transaction.

### `tao::pq::transaction`

The base class for all transactions. Like connections, transactions are handled via `std::shared_ptr`. SQL commands are always executed within a transaction with the `execute()` method. You can also get a subtransacion by calling `subtransaction()`. The subtransaction is, again, a transaction and you can create nested subtransactions. The library will ensure that transactions are used with correct nesting, for each transaction there is only one active transaction that can be used. For each transaction instance you should call either `commit()` or `rollback()`, otherwise the dtor will try to call `rollback()`. After calling either `commit()` or `rollback()`, a transaction is automatically marked inactive. Usage of inactive transactions results in an exception being thrown.

### Execution of SQL statements

Executing an SQL statement is done by calling `execute()` on either a transaction or on a connection directly. Calling it on a connection is equivalent of creating an auto-commit transaction and forwarding the `execute()` call to that transaction. Transaction order/nesting is checked before any statement is executed.

The statement may contain placeholders `$1`, `$2`, ... which are passed as additional parameters to `execute()`. By *not* concatenating parameters into the SQL statement string, you gain two major advantages: It is more efficient and you don't have to worry about SQL injection attacks - the latter is simply not possible when using placeholders.

### Prepared statements

On a connection, you can prepare statements with a symbolic name by calling `prepare()`. Simply pass the symbolic name instead of the actual SQL command to `execute()` to call the prepared statement, including the parameters for the placeholders. A prepared statement can be un-prepared by calling `deallocate()`.

### `tao::pq::result`

Each execution of an SQL command returns a result instance. A result is a non-modifiable container which contains either the number of rows affected by an INSERT/UPDATE/DELETE statement or any number of rows returned from a SELECT statement. It is usually clear which type of result is returned from a statement, in case of doubt (or for a more generic result handler), you can call `has_rows_affected()`.

### `tao::pq::row`

A single row from a result. Each row has one or more fields, accessible either by index or by name.

### `tao::pq::field`

A single field from a result row. Each field can be converted to different data types.

### SQL `NULL`

SQL `NULL` can be supplied as `tao::pq::null` as a parameter to a statement and it can be mapped to `std::optional` for results.

### Data Types

A set of predefined data types is available for both parameters (converting a type `T` into a string for PostgreSQL's API) and for results (converting a string returned from the database into a type `T`). The user can specialize both `tao::pq::parameter_traits` as well as `tao::pq::result_traits` to extend the API. Each data type may correspond to one or more result fields.

Copyright (c) 2016-2018 Daniel Frey and Dr. Colin Hirsch
