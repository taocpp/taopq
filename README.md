# The Art of C++ / PostgreSQL

[![Release](https://img.shields.io/github/release/taocpp/postgres.svg)](https://github.com/taocpp/postgres/releases/latest)
[![TravisCI](https://travis-ci.org/taocpp/postgres.svg)](https://travis-ci.org/taocpp/postgres)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/github/taocpp/postgres?svg=true)](https://ci.appveyor.com/project/taocpp/postgres)
[![Coverage](https://img.shields.io/coveralls/taocpp/postgres.svg)](https://coveralls.io/github/taocpp/postgres)

The Art of C++ / PostgreSQL is a light-weight C++17 client library for accessing a PostgreSQL database, providing a modern C++ wrapper for `libpq`.
It has no dependencies beyond a C++17 compatible compiler and the PostgreSQL `libpq` library.

## Documentation

 * [Version 1.x](doc/README.md) (requires C++17)

## Example

Here is a small "getting started" example.
The library has many more features and possibilities to offer.
The "best practice" often diverges from the style of this introductory example.

```c++
#include <tao/postgres.hpp>

#include <cassert>
#include <iostream>

int main()
{
  // open a connection
  const auto conn = tao::postgres::connection::create( "dbname=template1" );

  // execute statements directly
  conn->execute( "DROP TABLE IF EXISTS tao_example" );
  conn->execute( "CREATE TABLE tao_example ( a INTEGER PRIMARY KEY, b INTEGER, c TEXT NOT NULL )" );

  // preparing a statement is optional, but often recommended
  conn->prepare( "insert", "INSERT INTO tao_example VALUES ( $1, $2, $3 )" );

  // use a transaction if needed
  {
    const auto tr = conn->transaction();

    // execute statement with parameters directly
    tr->execute( "INSERT INTO tao_example VALUES ( $1, $2, $3 )", 1, 42, "foo" );

    // execute prepared statement with parameters
    tr->execute( "insert", 2, tao::postgres::null, "Hello, world!" );

    tr->commit();
  }

  // insert/update/delete statements return a result which can be queried for the rows affected
  {
    const auto res = conn->execute( "insert", 3, 3, "drei" );
    assert( res.rows_affected() == 1 );
  }

  // queries have a result as well, it contains the returned data
  const auto res = conn->execute( "SELECT * FROM tao_example" );
  assert( res.size() == 3 );

  // iterate over a result
  for( const auto& row : res ) {
    // access fields by index or (less efficiently) by name
    std::cout << row[ 0 ].as< int >() << ": " << row[ "c" ].as< std::string >() << std::endl;
  }

  // or convert a result into a container
  const auto v = res.vector< std::tuple< int, std::optional< int >, std::string > >();
}
```

## Contact

The Art of C++ / Postgres is part of [The Art of C++].

For questions and suggestions about The Art of C++ / Postgres please contact the authors at `taocpp(at)icemx.net`.

## License

The Art of C++ / Postgres is certified [Open Source](http://www.opensource.org/docs/definition.html) software.
It may be used for any purpose, including commercial purposes, at absolutely no cost.
It is distributed under the terms of the [MIT license](http://www.opensource.org/licenses/mit-license.html) reproduced here.

> Copyright (c) 2016-2018 Daniel Frey
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[The Art of C++]: https://taocpp.github.io/
