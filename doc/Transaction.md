# Transaction

Before we continue with our own documentation, we'd like to once again point you to the excellent [PostgreSQL documentation➚](https://www.postgresql.org/docs/current/tutorial-transactions.html) on transactions.
We will assume that you are familiar with how transactions work on PostgreSQL and in general, so we will *not* repeat this here.

## Synopsis

```c++
namespace tao::pq
{
   namespace internal
   {
      class zsv;  // zero-terminated string view
   }

   class connection;
   class result;

   class transaction
      : public std::enable_shared_from_this< transaction >
   {
   public:
      // non-copyable, non-movable
      transaction( const transaction& ) = delete;
      transaction( transaction&& ) = delete;
      void operator=( const transaction& ) = delete;
      void operator=( transaction&& ) = delete;

      virtual ~transaction() = default;

      // create transactions
      auto subtransaction()
         -> std::shared_ptr< transaction >;

      // asynchronous statement execution
      template< typename... As >
      void send( const internal::zsv statement, As&&... as );

      // asynchronous result retrieval
      auto get_result() -> result;

      // synchronous statement execution
      template< typename... As >
      auto execute( const internal::zsv statement, As&&... as )
      {
         send( statement, std::forward< As >( as )... );
         return get_result();
      }

      // finalize
      void commit();
      void rollback();

      // access connection
      auto connection() const noexcept
         -> const std::shared_ptr< pq::connection >&;
   };
}
```

:point_up: Note that `tao::pq::internal::zsv` is explained in the [Statement](Statement.md) chapter.

## Creating Transactions

In taoPQ, you create a top-level transaction from a connection, the methods available to do so are described in the [Connection](Connection.md) chapter.
In short, you create a normal transaction with the connection's `transaction()`-method or a "direct transaction" with the connection's `direct()`-method.
Both return a shared pointer to a `tao::pq::transaction`-derived object.

From any transaction, you can create a subtransaction by calling the `subtransaction()`-method.

```c++
auto tao::pq::transaction::subtransaction()
   -> std::shared_ptr< tao::pq::transaction >;
```

It returns just another `tao::pq::transaction`-derived object from which you may create further, nested subtransactions if needed.

All transactions then offer the above, unified interface.

## Statement Execution

On all transactions you can execute SQL statements.
If you execute a statement on a connection object directly, is creates an implicit direct transaction and forwards the execution to that temporary transaction.
The actual statement execution, i.e. the `execute()`-method, is described in the [Statement](Statement.md) chapter.

## Terminate Transaction

Transaction can be terminated in one of two ways.

### Commit a Transaction

In order to commit a transaction you call the `commit()`-method.

```c++
void tao::pq::transaction::commit();
```

All changes made by the transaction become visible to others and are guaranteed to be durable if a crash occurs.

### Abort a Transaction

In order to abort a transaction you call the `rollback()`-method.

```c++
void tao::pq::transaction::rollback();
```

Rolls back the current transaction and causes all the updates made by the transaction to be discarded.

## Transaction Ordering

Any transactions created via taoPQ is registered in the connection object as the currently active transaction.
At any given time, a connection can only have a single active transaction.
If you attempt to use a transaction object in the wrong order, taoPQ will notice and throw an appropriate `std::logic_error` exception.

:point_up: Note that the correct order depends on the *logical* lifetime of transactions.
The logical lifetime of a transactions ends when you explicitly call either the `commit()`- or the `rollback()`-method, or if the object's lifetime ends.
The destructor will automatically perform a call to the `rollback()`-method if the lifetime was not ended explicitly.
This comes in handy when exceptions are thrown and the destructor call happens due to the associated stack unwinding.

## Direct Transactions

Without an active transaction PostgreSQL works in ["autocommit"➚](https://www.postgresql.org/docs/current/sql-begin.html) mode.
This means that each statement is executed in its own internal transaction and a commit is implicitly performed at the end of the statement (if execution was successful, otherwise a rollback is done).
A "direct transaction" represents this concept in taoPQ.

A direct transaction is special, as the `commit()`- and `rollback()`-methods are available, but normally not needed.
Specifically, you don't need to call the `commit()`-method in order to make the changes you made permanent.
However, calling the `commit()`-method ends the logical lifetime of the transaction and the transaction deregisters itself from the connection.
In generic code you might receive a transaction from somewhere else and you might call the `commit()`-method regardless of whether the underlying transaction is a direct transaction or a normal transaction.

In case you need to know whether a given transaction object is a direct transaction or not, you can call the `is_direct()`-method.

```c++
bool tao::pq::transaction::is_direct() const noexcept;
```

Opening a subtransaction from a direct connection is possible and simply starts a normal transaction on the connection object.

## Manual Transaction Handling

You can manually begin, commit, or rollback transactions by executing [`BEGIN`➚](https://www.postgresql.org/docs/current/sql-begin.html), [`COMMIT`➚](https://www.postgresql.org/docs/current/sql-commit.html), or [`ROLLBACK`➚](https://www.postgresql.org/docs/current/sql-rollback.html) statements directly via the `execute()`-method.
Likewise, you can manually create, commit, or rollback subtransactions by executing [`SAVEPOINT`➚](https://www.postgresql.org/docs/current/sql-savepoint.html), [`RELEASE SAVEPOINT`➚](https://www.postgresql.org/docs/current/sql-release-savepoint.html), or [`ROLLBACK TO SAVEPOINT`➚](https://www.postgresql.org/docs/current/sql-rollback-to.html) statements directly via the `execute()`-method.

:point_up: We strongly advise against manual transaction handling, as it will not be tracked by taoPQ and might confuse our library's transaction ordering framework.
We advise to use the methods offered by taoPQ instead of manually handling transactions.

## Accessing the Connection

If you need to access the connection that a transaction is bound to, you can call the `connection()`-method.

```c++
auto tao::pq::transaction::connection() const noexcept
   -> const std::shared_ptr< tao::pq::connection >&;
```

:point_up: Note that the shared pointer will be empty if the logical lifetime of the transaction ended.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2022 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
