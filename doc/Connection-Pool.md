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
   class connection;
   class result;

   class connection_pool final
      : public std::enable_shared_from_this< connection_pool >
   {
   public:
      // create a new connection pool
      static auto create( const std::string& connection_info )
         -> std::shared_ptr< connection_pool >;

      // non-copyable, non-movable
      connection_pool( const connection_pool& ) = delete;
      connection_pool( connection_pool&& ) = delete;
      void operator=( const connection_pool& ) = delete;
      void operator=( connection_pool&& ) = delete;

      virtual ~connection_pool() = default;

      // borrow a connection
      auto connection() const noexcept
         -> std::shared_ptr< pq::connection >;

      // direct statement execution
      template< typename... As >
      auto execute( const char* statement, As&&... as )
         -> result;

      template< typename... As >
      auto execute( const std::string& statement, As&&... as )
         -> result;

      // cleanup
      void erase_invalid();
   };
}
```

**TODO**

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
