# Connection

All communication with a database server is handled through a connection, represented by the `tao::pq::connection` type in taoPQ.
A connection object takes care of tracking [transactions](Transaction.md), [error handling](Error-Handling.md), and it has its own set of prepared statements.

## Synopsis

```c++
namespace tao::pq
{
   namespace internal
   {
      class zsv;  // zero-terminated string view
   }

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

   class notification final
   {
   public:
      auto channel() const noexcept -> const char*;
      auto payload() const noexcept -> const char*;

      auto underlying_raw_ptr() noexcept -> PGnotify*;
      auto underlying_raw_ptr() const noexcept -> const PGnotify*;
   };

   class transaction;

   class connection final
      : public std::enable_shared_from_this< connection >
   {
   public:
      // create a new connection
      static auto create( const std::string& connection_info )
         -> std::shared_ptr< connection >;

      // non-copyable, non-movable
      connection( const connection& ) = delete;
      connection( connection&& ) = delete;
      void operator=( const connection& ) = delete;
      void operator=( connection&& ) = delete;

      ~connection() = default;

      // query status
      bool is_open() const noexcept;
      bool is_idle() const noexcept;

      // create transactions
      auto direct()
         -> std::shared_ptr< pq::transaction >;

      auto transaction()
         -> std::shared_ptr< pq::transaction >;

      auto transaction( const access_mode am,
                        const isolation_level il = isolation_level::default_isolation_level )
         -> std::shared_ptr< pq::transaction >;

      auto transaction( const isolation_level il,
                        const access_mode am = access_mode::default_access_mode )
         -> std::shared_ptr< pq::transaction >;

      // timeout handling
      auto timeout() const noexcept
         -> const std::optional< std::chrono::milliseconds >&;

      void set_timeout( const std::chrono::milliseconds timeout );
      void reset_timeout() noexcept;

      // prepared statements
      void prepare( const std::string& name, const std::string& statement );
      void deallocate( const std::string& name );

      // direct statement execution
      template< typename... As >
      auto execute( const internal::zsv statement, As&&... as )
      {
         return direct()->execute( statement, std::forward< As >( as )... );
      }

      // listen/notify support
      void listen( const std::string_view channel );
      void listen( const std::string_view channel, const std::function< void( const char* ) >& handler );

      void unlisten( const std::string_view channel );

      void notify( const std::string_view channel );
      void notify( const std::string_view channel, const std::string_view payload );

      auto notification_handler()
         -> std::function< void( const notification& ) >;

      void set_notification_handler( const std::function< void( const notification& ) >& handler );
      void reset_notification_handler() noexcept;

      auto notification_handler( const std::string_view channel )
         -> std::function< void( const char* ) >;

      void set_notification_handler( const std::string_view channel, const std::function< void( const char* ) >& handler );
      void reset_notification_handler( const std::string_view channel ) noexcept;

      void handle_notifications();
      void get_notifications();

      // access underlying connection pointer from libpq
      auto underlying_raw_ptr() noexcept -> PGconn*;
      auto underlying_raw_ptr() const noexcept -> const PGconn*;

      // access the socket used by libpq
      auto socket() const -> int;

      // error message
      auto error_message() const -> std::string;
   };
}
```

:point_up: Note that `tao::pq::internal::zsv` is explained in the [Statement](Statement.md) chapter.

## Creating a Connection

A connection is created by calling `tao::pq::connection`'s static `create()`-method.

```c++
auto tao::pq::connection::create( const std::string& connection_info )
    -> std::shared_ptr< tao::pq::connection >;
```

It takes a single parameter, the [connection string➚](https://www.postgresql.org/docs/current/libpq-connect.html#LIBPQ-CONNSTRING), that is passed to the underlying `libpq` for opening the database connection.
The connection string contains parameters and options, such as the server address or the database name.
Connection parameters that are not specified in the connection string might also be set via [environment variables➚](https://www.postgresql.org/docs/current/libpq-envars.html).

The method returns a `std::shared_ptr<tao::pq::connection>` or, in case of an error, throws an exception.
When the last reference to a connection is deleted, i.e. the last shared pointer referencing it is deleted or reset, the connection is closed via its destructor which takes care of freeing underlying resources.
The shared pointer might also be stored internally in other objects of taoPQ, i.e. a transaction.
This ensures, that the connection is kept alive as long as there are dependent objects like an active transaction, see below.

## Creating Transactions

You can create [transactions](Transaction.md) by calling either the `direct()`-method or the `transaction()`-method.
Both methods register the newly created transaction as the active transaction of a connection.
A connection can only have one active transaction at any given time.
Further details on how to use transactions are discussed in the [Transaction](Transaction.md) chapter.

### Creating a "Direct" Transaction

The `direct()`-method creates an auto-commit transaction proxy, i.e. all statements executed on this transaction are immediately committed to the database.

```c++
auto tao::pq::connection::direct()
    -> std::shared_ptr< tao::pq::transaction >;
```

:point_up: This is not a real transaction from the database's point of view, therefore calling the `commit()`- or `rollback()`-method on the transaction has no immediate effect on the database.
However, calling either the `commit()`- or `rollback()`-method will end the transaction's logical lifetime and it will unregister itself from the connection.

### Creating a Database Transaction

The `transaction()`-method begins a real [database transaction➚](https://www.postgresql.org/docs/current/tutorial-transactions.html).

```c++
auto tao::pq::connection::transaction()
    -> std::shared_ptr< tao::pq::transaction >;

auto tao::pq::connection::transaction( const tao::pq::isolation_level il,
                                       const tao::pq::access_mode am = tao::pq::access_mode::default_access_mode )
    -> std::shared_ptr< tao::pq::transaction >;

auto tao::pq::connection::transaction( const tao::pq::access_mode am,
                                       const tao::pq::isolation_level il = tao::pq::isolation_level::default_isolation_level )
    -> std::shared_ptr< tao::pq::transaction >;
```

You may specify two optional parameters, the [isolation level➚](https://www.postgresql.org/docs/current/transaction-iso.html) and the [access mode➚](https://www.postgresql.org/docs/current/sql-set-transaction.html).

When `tao::pq::isolation_level::default_isolation_level` or `tao::pq::access_mode::default_access_mode` are used the transaction inherits its isolation level or access mode from the session, as described in the [PostgreSQL documentation➚](https://www.postgresql.org/docs/current/sql-set-transaction.html).

## Executing Statements

You can [execute statements](Statement.md) on a connection object directly, which is equivalent to creating a temporary direct transaction (as if calling the `direct()`-method) and executing the statement on that [transaction](Transaction.md).

## Prepared Statements

Prepared statements only last for the duration of a connection, and are bound to a connection, i.e. the set of prepared statements is independent for each connection.

You can [prepare➚](https://www.postgresql.org/docs/current/sql-prepare.html) a statement by calling the `prepare()`-method.

```c++
void tao::pq::connection::prepare( const std::string& name, const std::string& statement );
```

It takes two parameters, the name of the prepared statement and the SQL statement itself.
taoPQ limits the name to classic C-style identifiers, i.e. a non-empty sequence of digits, underscores, and lowercase and uppercase Latin letters.
A valid identifier must begin with a non-digit character.
Identifiers are case-sensitive (lowercase and uppercase letters are distinct).

A previously prepared statement can be [deallocated➚](https://www.postgresql.org/docs/current/sql-deallocate.html), although this is rare in pratice.
To deallocate a prepared statement, call the `deallocate()`-method.

```c++
void tao::pq::connection::deallocate( const std::string& name );
```

Using the `prepare()`- and `deallocate()`-methods makes taoPQ's connection object aware of the names of the prepared statements.
This allows the [execution](Statement.md) of those prepared statements transparently via an `execute()`-method.

### Manually Prepared Statements

You can manually prepare statements by executing [`PREPARE`➚](https://www.postgresql.org/docs/current/sql-prepare.html) statements directly via an `execute()`-method.
While those prepared statements live on the same connection, there are some important differences.
You can only execute those prepared statements by executing [`EXECUTE`➚](https://www.postgresql.org/docs/current/sql-execute.html) statements directly via an `execute()`-method, and you can only deallocate them by executing [`DEALLOCATE`➚](https://www.postgresql.org/docs/current/sql-deallocate.html) statements directly via an `execute()`-method.

:point_up: We advise to use the methods offered by taoPQ's connection type.

## Checking Status

You can check a connection's status by calling the `is_open()`- or `is_idle()`-methods.

```c++
bool tao::pq::connection::is_open() const noexcept;
bool tao::pq::connection::is_idle() const noexcept;
```

The first method returns `true` when the connection is still open and usable, and `false` otherwise, i.e. if the connection is in a failed state.
For further details, check the documentation for the underlying [`PQstatus()`➚](https://www.postgresql.org/docs/current/libpq-status.html)-function provided by `libpq`.

The second method returns `true` when the connection is open and is in the idle state, and `false` otherwise.
For further details, check the documentation for the underlying [`PQtransactionStatus()`➚](https://www.postgresql.org/docs/current/libpq-status.html)-function provided by `libpq`.

## Notification Framework

PostgreSQL provides a simple [interprocess communication mechanism➚](https://www.postgresql.org/docs/current/sql-notify.html) for a collection of applications accessing the same database.

### Sending Messages

You can send events with the `notify()`-method, providing a channel name and optionally a payload as the second parameter.

```c++
void tao::pq::connection::notify( const std::string_view channel );
void tao::pq::connection::notify( const std::string_view channel, const std::string_view payload );
```

:point_up: The channel name is case sensitive when using taoPQ's methods.

### Receiving Messages

You can subscribe to channels to receive messages using the `listen()`-method, or unsubscribe by calling the `unlisten()`-method.

```c++
void tao::pq::connection::listen( const std::string_view channel );
void tao::pq::connection::unlisten( const std::string_view channel );
```

Note that subscriptions are per connection.

### Handling Messages

Processing received messages requires you to register a notification handler.
Each connection has its own notification handler.
The notification handler is managed by a `std::function< void( const tao::pq::notification& >` object.

The currently active notification handler is returned by the `notification_handler()`-method.

```c++
auto tao::pq::connection::notification_handler()
   -> std::function< void( const tao::pq::notification& ) >;
```

If no notification handler is set, the [`std::function`➚](https://en.cppreference.com/w/cpp/utility/functional/function) will be empty.

Setting a notification handler is done by calling the `set_notification_handler()`-method.

```c++
void tao::pq::connection::set_notification_handler( const std::function< void( const tao::pq::notification& ) >& handler );
```

If you want to deregister the current notification handler, you can call the `reset_notification_handler()`-method.

```c++
void tao::pq::connection::reset_notification_handler() noexcept;
```

### Per Channel Handlers

Besides the above general notification handler, there is also the option to register a per channel handler.
Per channel handlers only receive the payload as a parameter.

```c++
auto tao::pq::connection::notification_handler( const std::string_view channel )
   -> std::function< void( const char* ) >;

void tao::pq::connection::set_notification_handler( const std::string_view channel,
                                                    const std::function< void( const char* ) >& handler );
void tao::pq::connection::reset_notification_handler( const std::string_view channel ) noexcept;
```

When you subscribe to a channel with the `listen()`-method, you can optionally register a channel handler.

```c++
void tao::pq::connection::listen( const std::string_view channel,
                                  const std::function< void( const char* ) >& handler );
```

This registers the handler first by calling `set_notification_handler( channel, handler )`, then calls `listen( channel )`.

### Asynchronous Notifications

taoPQ calls the registered notification handler(s) after successful execution by calling the `handle_notifications()`-method.
As a user, you rarely need to call the `handle_notifications()`-method manually.

```c++
void tao::pq::connection::handle_notifications();
```

When you don't have any statement to execute, you can call the `get_notifications()`-method which will actively query the server for new events.

```c++
void tao::pq::connection::get_notifications();
```

### Event Loop

**TODO** Support event loops? How?

## Underlying Connection Pointer

If you need to access the underlying raw connection pointer from `libpq`, you can call the `underlying_raw_ptr()`-method.

```c++
auto tao::pq::connection::underlying_raw_ptr() noexcept -> PGconn*;
auto tao::pq::connection::underlying_raw_ptr() const noexcept -> const PGconn*;
```

## Error Messages

You can retrieve the last error message (if applicable) by calling the `error_message()`-method.

```c++
auto tao::pq::connection::error_message() const -> std::string;
```

When taoPQ throws an exception this is usually done internally and the message is part of the exception's `what()` message.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2022 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
