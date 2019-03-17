# Welcome to taoPQ

[![Release](https://img.shields.io/github/release/taocpp/taopq.svg)](https://github.com/taocpp/taopq/releases/latest)
[![TravisCI](https://travis-ci.org/taocpp/taopq.svg)](https://travis-ci.org/taocpp/taopq)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/github/taocpp/taopq?svg=true)](https://ci.appveyor.com/project/taocpp/taopq)
[![Coverage](https://img.shields.io/coveralls/taocpp/taopq.svg)](https://coveralls.io/github/taocpp/taopq)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/taocpp/taopq.svg)](https://lgtm.com/projects/g/taocpp/taopq/context:cpp)

taoPQ is a light-weight C++ client library for accessing a PostgreSQL database, providing a modern wrapper for `libpq`.
It has no dependencies beyond a C++17 compatible compiler and the PostgreSQL `libpq` client library.

## Documentation

 * [Version 1.x](doc/README.md) (requires C++17)

## Example

The following is an introductory example to "get started" and to show the basic look-and-feel.
The library offers many more features and possibilities.
What is considered "best practice" often diverges from the style shown below.

```c++
#include <tao/pq.hpp>

#include <cassert>
#include <iostream>

int main()
{
  // Open a connection
  const auto conn = tao::pq::connection::create( "dbname=template1" );

  // Directly execute SQL statements
  conn->execute( "DROP TABLE IF EXISTS taopq_example" );
  conn->execute( "CREATE TABLE taopq_example ( a INTEGER PRIMARY KEY, b INTEGER, c TEXT NOT NULL )" );

  // Prepare a statement
  conn->prepare( "insert", "INSERT INTO taopq_example VALUES ( $1, $2, $3 )" );

  // Begin a transaction
  {
    const auto tr = conn->transaction();

    // Execute statement with parameters
    tr->execute( "INSERT INTO taopq_example VALUES ( $1, $2, $3 )", 1, 42, "foo" );

    // Execute previously prepared statement with parameters (recommended)
    tr->execute( "insert", 2, tao::pq::null, "Hello, world!" );

    // Commit the transaction (rollback() available, too)
    tr->commit();
  }

  // insert/update/delete statements return an object that contains the number of rows affected
  {
    const auto res = conn->execute( "insert", 3, 3, "drei" );
    assert( res.rows_affected() == 1 );
  }

  // Queries return an object that contains the result set
  const auto res = conn->execute( "SELECT * FROM taopq_example" );
  assert( res.size() == 3 );

  // Iterate over a query result...
  for( const auto& row : res ) {
    // Access fields by index (faster) or by name (slower)
    std::cout << row[ 0 ].as< int >() << ": " << row[ "c" ].as< std::string >() << std::endl;
  }

  // ... or convert it into a container
  const auto v = res.vector< std::tuple< int, std::optional< int >, std::string > >();
  
  // More advanced use cases include support for custom data types and nested transactions
}
```

## Contact

taoPQ is part of [The Art of C++].

For questions and suggestions about taoPQ please contact the authors at `taocpp(at)icemx.net`.

## License

taoPQ is certified [Open Source](http://www.opensource.org/docs/definition.html) software.
It may be used for any purpose, including commercial purposes, at absolutely no cost.
It is distributed under the terms of the [MIT license](http://www.opensource.org/licenses/mit-license.html) reproduced here.

> Copyright (c) 2016-2019 Daniel Frey
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[The Art of C++]: https://taocpp.github.io/
