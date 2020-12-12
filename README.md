# Welcome to taoPQ

[![Windows CI](https://github.com/taocpp/taopq/workflows/Windows/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3AWindows)
[![macOS CI](https://github.com/taocpp/taopq/workflows/macOS/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3AmacOS)
[![Linux CI](https://github.com/taocpp/taopq/workflows/Linux/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3ALinux)
<br>
[![clang-analyze](https://github.com/taocpp/taopq/workflows/clang-analyze/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3Aclang-analyze)
[![clang-tidy](https://github.com/taocpp/taopq/workflows/clang-tidy/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3Aclang-tidy)
[![Sanitizer](https://github.com/taocpp/taopq/workflows/Sanitizer/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3ASanitizer)
[![CodeQL](https://github.com/taocpp/taopq/workflows/CodeQL/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3ACodeQL)
[![Code Coverage](https://codecov.io/gh/taocpp/taopq/branch/master/graph/badge.svg?token=ykWa8RRdyk)](https://codecov.io/gh/taocpp/taopq)

taoPQ is a light-weight C++ client library for accessing a PostgreSQL database, providing a modern wrapper for `libpq`.
It has no dependencies beyond a C++17 compatible compiler and the PostgreSQL `libpq` client library.

## Documentation

 * [Version 1.x](doc/README.md)

## Introduction

This library provides classes for database connections and transactions, as well as connection pools and table writers.
It also supports for prepared statements and nested transaction.
An extensible traits mechanism is used to convert C++ types into SQL statement parameters, and conversely to convert query results into arbitrary C++ types.
The following example shows the basic look and feel of the library.

```c++
#include <cassert>
#include <iostream>

#include <tao/pq.hpp>

int main()
{
  // Open connection
  const auto conn = tao::pq::connection::create( "dbname=template1" );

  // Execute SQL statement
  conn->execute( "DROP TABLE IF EXISTS taopq_example" );
  conn->execute( "CREATE TABLE taopq_example ( a INTEGER PRIMARY KEY, b INTEGER, c TEXT NOT NULL )" );

  // Prepare statement
  conn->prepare( "my_stmt", "INSERT INTO taopq_example VALUES ( $1, $2, $3 )" );

  {
    // Begin transaction
    const auto tr = conn->transaction();

    // Execute statement with parameters
    tr->execute( "INSERT INTO taopq_example VALUES ( $1, $2, $3 )", 1, 42, "foo" );

    // Execute previously prepared statement with parameters (recommended)
    tr->execute( "my_stmt", 2, tao::pq::null, "Hello, world!" );

    // Commit transaction
    tr->commit();
  }

  // insert/update/delete statements return an object that contains the number of rows affected
  {
    const auto res = conn->execute( "insert", 3, 3, "drei" );
    assert( res.rows_affected() == 1 );
  }

  // Queries return object with result set
  const auto res = conn->execute( "SELECT * FROM taopq_example" );
  assert( res.size() == 3 );

  // Conveniently convert result into C++ container
  const auto v = res.vector< std::tuple< int, std::optional< int >, std::string > >();
}
```

## Contact

taoPQ is part of [The Art of C++].

For questions and suggestions about taoPQ please contact the authors at `taocpp(at)icemx.net`.

## License

taoPQ is certified [Open Source](http://www.opensource.org/docs/definition.html) software.
It may be used for any purpose, including commercial purposes, at absolutely no cost.
It is distributed under the terms of the [MIT license](http://www.opensource.org/licenses/mit-license.html) reproduced here.

> Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[The Art of C++]: https://taocpp.github.io/
