# Connection

All communication with a database server is handled through a connection, represented by the `tao::pq::connection` type in taoPQ.
A connection object takes care of [error handling](Error-Handling.md), tracking [transactions](Transaction.md), and it has its own set of [prepared statements](Prepared-Statements.md).

## Creating a Connection

A connection is created by calling `tao::pq::connection`'s static `create()`-method.

It takes a single parameter, the [connection string](https://www.postgresql.org/docs/current/libpq-connect.html#LIBPQ-CONNSTRING), that is passed to the underlying `libpq` for opening the database connection.
The connection string contains parameters and options, such as the server address or the database name.
Connection parameters that are not specified in the connection string might also be set via [environment variables](https://www.postgresql.org/docs/current/libpq-envars.html).

The method returns a `std::shared_ptr< tao::pq::connection >` or, in case of an error, throws an exception.
When the last reference to a connection is deleted, i.e. the last `std::shared_ptr` pointing to it is deleted or reset, the connection is closed via its destructor which takes care of freeing underlying resources.
The shared pointer might also be stored internally in other objects of taoPQ, i.e. a transaction.
This ensures, that the connection is kept alive as long as there are dependent objects like an active connection, see below.

## Creating Transactions

You can create [transactions](Transaction.md) by calling either the `direct()`-method or the `transaction()`-method.
Both methods register the newly created transaction as the active transaction of a connection.
A connection can only have one active transaction at any given time.
Further details on how to use transactions is discussed in the [Transaction](Transaction.md) chapter.

### Creating a "Direct" Transaction

The `direct()`-method creates an auto-commit transaction proxy, i.e. all statements executed on this transaction are immediately committed to the database.
This is not a real transaction from the database's point of view, therefore calling `commit()` or `rollback()` on the transaction has no immediate effect on the database.
However, calling either `commit()` or `rollback()` will end the transaction's logical lifetime and it will unregister itself from the connection.

### Creating a Database Transaction

The `transaction()`-method creates a real [database transaction](https://www.postgresql.org/docs/current/tutorial-transactions.html).
You may specify two optional parameters, the [isolation level](https://www.postgresql.org/docs/current/transaction-iso.html) and the [access mode](https://www.postgresql.org/docs/current/sql-set-transaction.html).
taoPQ defines these as enumeration types as follows:

```c++
namespace tao::pq
{
   enum class isolation_level
   {
      default_isolation_level,
      serializable,
      repeatable_read,
      read_committed,
      read_uncommitted
   };

   enum class access_mode
   {
      default_access_mode,
      read_write,
      read_only
   };
}
```

When `tao::pq::isolation_level::default_isolation_level` or `tao::pq::access_mode::default_access_mode` are used the transaction inherits its isolation level or access mode from the session, as described in the [PostgreSQL documentation](https://www.postgresql.org/docs/current/sql-set-transaction.html).

## Executing Statements

You can execute statements on a connection object directly, which is equivalent to creating a temporary direct transaction (as if calling the `direct()`-method) and [executing the statement](Statement.md) on that [transaction](Transaction.md).

## Prepared Statements

[Prepared statements](Prepared-Statements.md) only last for the duration of a connection, and are bound to a connection, i.e. the set of prepared statements is independent for each connection.

You can [prepare](https://www.postgresql.org/docs/current/sql-prepare.html) a statement by calling the `prepare()`-method.
It takes two parameters, the name of the prepared statement and the SQL statement itself.
taoPQ limits the name to classic C-style identifiers, i.e. a non-empty sequence of digits, underscores, and lowercase and uppercase Latin letters.
A valid identifier must begin with a non-digit character.
Identifiers are case-sensitive (lowercase and uppercase letters are distinct).

A previously prepared statement can be [deallocated](https://www.postgresql.org/docs/current/sql-deallocate.html), although this is rare in pratice.
To deallocate a prepared statement, call the `deallocate()`-method.
It takes the name of the prepared statement as its only parameter.

Using the `prepare()`- and `deallocate()`-methods makes taoPQ's connection object aware of the names of the prepared statements.
This allows the [execution](Statement.md) of those prepared statements transparently via `execute()`.

### Manually Prepared Statements

You can manually prepare statements by executing [`PREPARE`](https://www.postgresql.org/docs/current/sql-prepare.html) statements directly via `execute()`.
While those prepared statements live on the same connection, there are some important differences.
You can only execute those prepared statements by executing [`EXECUTE`](https://www.postgresql.org/docs/current/sql-execute.html) statements directly via `execute()`, and you can only deallocate them by executing [`DEALLOCATE`](https://www.postgresql.org/docs/current/sql-deallocate.html) statements directly via `execute()`.

We advise to use the methods offered by taoPQ's connection type.

## Checking Status

You can check a connection's status by calling the `is_open()` method.
It return `true` when the connection is still open and usable, and `false` otherwise, i.e. if the connection is in a failed state.
For further details, check the documentation for the underlying [`PQstatus`](https://www.postgresql.org/docs/current/libpq-status.html)-function provided by `libpq`.

## Parameter Traits

The above is slightly inaccurate, as the connection class of taoPQ is actually a class template, that takes a parameter traits class template as its parameter.
For now, you can ignore the meaning of the traits class template, as it will be explained later in its own chapter.
However, it means that `tao::pq::connection` is actually a type alias for `tao::pq::basic_connection< tao::pq::parameter_text_traits >`.
The traits class for the parameters is used when executing statements with parameters and it is passed on to the transactions created from a connection object.
More precisely, the connection's traits class template is used as a default when no traits class template is explicitly specified for a call to the `direct()`-, `transaction()`-, or `execute()`-method.
The specify a different traits class template, simply provide it as a template parameter to the method, e.g. `direct< my_traits >()`.

TODO: Do we really want to keep binary and text format traits? It complicates the library as well as the documentation significantly and shifts a lot of code into headers.

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
