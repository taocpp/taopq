# Connection Pool

Opening a new connection to the database server typically consists of several time-consuming steps.
A physical channel such as a socket or a named pipe must be established, the initial handshake with the server must occur, if encryption protocols (SSL) are enabled those need to be established, the connection string information must be parsed, the connection must be authenticated by the server, checks must be run for enlisting in the current transactions, and so on.

In practice, most applications use only one or a few different configurations for connections.
This means that during application execution, many identical connections will be repeatedly opened and closed.
To minimize the cost of opening connections, taoPQ provides connection pools.

Connection pools reduces the number of times that new connections must be opened.
They manage connections by keeping alive a set of active connections and borrowing them to the application on demand.
The connection pool maintains ownership of the connections when they are not used by the application.

## Synopsis

```c++
namespace tao::pq
{
   namespace internal
   {
      class zsv;  // zero-terminated string view
   }

   namespace poll
   {
      enum class status
      {
         timeout,
         readable,
         writable,
         again
      };

      using callback = status( const int socket,
                               const bool wait_for_write,
                               const int timeout_ms );
   }

   class connection;

   class connection_pool final
      : public std::enable_shared_from_this< connection_pool >
   {
   public:
      // create a new connection pool
      static auto create( const std::string& connection_info,
                          std::function< tao::pq::poll::callback > poll_cb = /*unspecified*/ )
         -> std::shared_ptr< connection_pool >;

      // non-copyable, non-movable
      connection_pool( const connection_pool& ) = delete;
      connection_pool( connection_pool&& ) = delete;
      void operator=( const connection_pool& ) = delete;
      void operator=( connection_pool&& ) = delete;

      virtual ~connection_pool() = default;

      // timeout handling
      auto timeout() const noexcept
         -> const std::optional< std::chrono::milliseconds >&;

      void set_timeout( const std::chrono::milliseconds timeout );
      void reset_timeout() noexcept;

      // customizable poll()-callback
      auto poll_callback() const noexcept
         -> const std::function< tao::pq::poll::callback >&;

      void set_poll_callback( std::function< tao::pq::poll::callback > poll_cb ) noexcept;
      void reset_poll_callback();

      // borrow a connection
      auto connection() const noexcept
         -> std::shared_ptr< pq::connection >;

      // direct statement execution
      template< typename... As >
      auto execute( const internal::zsv statement, As&&... as )
      {
         return connection()->execute( statement, std::forward< As >( as )... );
      }

      // cleanup
      void erase_invalid();
   };
}
```

:point_up: Note that `tao::pq::internal::zsv` is explained in the [Statement](Statement.md) chapter.

## Creating Connection Pools

A connection pool is created by calling `tao::pq::connection_pool`'s static `create()`-method.

```c++
auto tao::pq::connection_pool::create( const std::string& connection_info )
    -> std::shared_ptr< tao::pq::connection_pool >;
```

It takes a mandatory parameter, the [connection string➚](https://www.postgresql.org/docs/current/libpq-connect.html#LIBPQ-CONNSTRING), that is used when new connections are opened by the pool.

The second, optional parameter can be used to specify the default `poll()`-callback for connections, see [Customizable `poll()`-callback](Connection.md#customizable-poll-callback).

## Borrowing Connections

When you need a connection, you simply call the `connection()`-method.

```c++
auto tao::pq::connection_pool::connection()
    -> std::shared_ptr< tao::pq::connection >;
```

This will either open a new connection when the pool is empty, or it will give you a reused connection from the pool.
As long as you retain ownership of the returned shared pointer, it is yours to work with.
When the last remaining shared pointer is destroyed or assigned another value, the connection is returned to the pool.

## Executing Statements

You can [execute statements](Statement.md) on a connection pool directly, which is equivalent to borrowing a temporary connection (as if calling the `connection()`-method) and executing the statement on that [connection](Connection.md).
After the statement was executed, the temporary connection is returned to the pool.

## Cleanup

The connection pool will implicitly discard connections that are in a failed state when they are returned to the pool or when they are retrieved from the pool.

In some environments you might need to periodically clean up the connection pool to get rid of connections that are no longer valid.
In order to do so, just call the `erase_invalid()`-method, which will check the status of each pooled connection and discard the invalid ones.

```c++
void tao::pq::connection_pool::erase_invalid();
```

## Customizable `poll()`-callback

The default implementation for polling uses `poll()` or `WSAPoll()`, depending on your system.
This callback can be customized to support other I/O frameworks, e.g. Boost.Asio.

To access the current default callback for borrowed connections you can call the `poll_callback()`-method.

```c++
auto poll_callback() const noexcept
   -> const std::function< tao::pq::poll::callback >&;
```

Setting the default `poll()`-callback for borrowed connections is done by calling the `set_poll_callback()`-method.

```c++
void set_poll_callback( std::function< tao::pq::poll::callback > poll_cb ) noexcept;
```

You can revert the current default `poll()`-callback for borrowed connections to the default by calling the `reset_poll_callback()`-method.

```c++
void reset_poll_callback();
```

## Thread Safety

The connection pool's borrowing mechanism is thread-safe, i.e. multiple threads can make calls to the `connection()`-method or return connections simultaneously.
You can also call the `erase_invalid()`-method at any time.

Internally, the connection pool uses a [mutex➚](https://en.cppreference.com/w/cpp/thread/mutex) to serialize the above operations.
We minimized the work in the [critical sections➚](https://en.wikipedia.org/wiki/Critical_section) as far as possible.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
