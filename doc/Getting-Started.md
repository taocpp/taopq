# Getting Started

Before we start with taoPQ, we'd like to point you to the excellent [PostgreSQL documentation](https://www.postgresql.org/docs/current/index.html).
We will assume that you are familiar with PostgreSQL and SQL in general, so we will *not* explain what a certain SQL statement does in the database.

Getting started with taoPQ is really simple, a minimalistic program looks as follows:

```c++
#include <iostream>
#include <string>

#include <tao/pq.hpp>

int main()
{
   const auto connection = tao::pq::connection::create( "dbname=template1" );
   const auto result = connection->execute( "SELECT version()" );
   std::cout << result.as< std::string >() << std::endl;
   return 0;
}
```

Let's go through the above code and explain some basic principles, slowly expanding our knowledge of taoPQ.

Starting with `tao::pq::connection::create("dbname=template1")`, we can see that the `tao::pq::connection` class has a static `create()`-method.
You provide a single parameter, the [connection string](https://www.postgresql.org/docs/current/libpq-connect.html#LIBPQ-CONNSTRING), and the method then returns a `std::shared_ptr<tao::pq::connection>` that holds the connection object it opened.
The reason why connections (and several other objects in taoPQ) are handled via smart pointers will be explained later.
Note that you can use `auto` as shown above to simplify your code.

A connection has an `execute()`-method which can be used to run SQL statements.
The `execute()`-method returns a result object directly, meaning `tao::pq::result` objects are not handled via smart pointers.

As the statement we just executed was a `SELECT` statement, the result contains a set of rows containing the data returned from the `SELECT` statement.
The above shows a simple way of converting a result set that contains only a single row with a single column into a C++ `std::string`.
Of course there are other, more sophisticated ways to retrieve and convert result data when you received multiple rows with multiple columns, those will be shown later.

Congratulations, you now can write simple programs with taoPQ.

## Next Steps

The following chapters are good next steps to get to know taoPQ:

* The [Connection](Connection.md) chapter goes into more details on the methods offered by `tao::pq::connection`, introduces transactions, and explains why some objects in taoPQ are handled via smart pointers.
* The [Transaction](Transaction.md) chapter explains in more detail how taoPQ handles transactions, makes sure that you don't mess up the transaction ordering and nesting, and which transaction types are supported.
* The [Statement](Statement.md) chapter gives more information on how to send statements and parameters.
* The [Result](Result.md) chapter explains what types of results exist, how you can access the data they contain, and how to convert results into C++ types, including containers.

* The [Installation](Installation.md) chapter explains how to install taoPQ.
* The [Requirements](Requirements.md) chapter lists our requirements and assumptions about the used server and protocol versions, encoding support, etc.

## Advanced Topics

* The [Error Handling](Error-Handling.md) chapter gives some general hints as to how we manage error scenarios and how those are communicated to the application.
* The [Prepared Statements](Prepared-Statements.md) chapter introduces [prepared statements](https://www.postgresql.org/docs/current/sql-prepare.html), a powerful way of improving your clients performance.
* The [Parameter Type Conversion](Parameter-Type-Conversion.md) chapter explains what C++ data types can be used as parameters when executing SQL statements, how NULL values are mapped to C++ data types, and how you can extend the supported types by registering your own types.
* The [Result Type Conversion](Result-Type-Conversion.md) chapter explains what C++ data types can be extracted from results, how you can extend the supported types by registering your own types, and how to use `tao::pq::result`'s API elegantly and efficiently.
* The [Binary Data](Binary-Data.md) chapter explains the support for PostgreSQL's [`BYTEA`](https://www.postgresql.org/docs/current/datatype-binary.html) data type in taoPQ and some design decisions regarding the C++ interface.
* The [Arrays](Arrays.md) chapter explains the support for PostgreSQL's [`ARRAY`](https://www.postgresql.org/docs/current/arrays.html) data types in taoPQ and some design decisions regarding the C++ interface.

* The [Connection Pool](Connection-Pool.md) chapter explains how a connection pool might help your application, especially when you use multi-threading. Our connection pool offers some novel features that ease the handling of borrowed connections significantly.
* The [Bulk Transfer](Bulk-Transfer.md) chapter explains how we support high-speed [bulk data transfer](https://www.postgresql.org/docs/current/sql-copy.html) to or from the server.
* The [Large Object](Large-Object.md) chapter provides access to PostgreSQL's [large object](https://www.postgresql.org/docs/current/largeobjects.html) facility.
* The [Performance](Performance.md) chapter gives hints on how to improve your application's performance, as well as explaining some gotchas you might encounter when using taoPQ.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
