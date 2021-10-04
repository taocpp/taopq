# Welcome to taoPQ

[![Windows CI](https://github.com/taocpp/taopq/workflows/Windows/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3AWindows)
[![macOS CI](https://github.com/taocpp/taopq/workflows/macOS/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3AmacOS)
[![Linux CI](https://github.com/taocpp/taopq/workflows/Linux/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3ALinux)
<br>
[![clang-analyze](https://github.com/taocpp/taopq/workflows/clang-analyze/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3Aclang-analyze)
[![clang-tidy](https://github.com/taocpp/taopq/workflows/clang-tidy/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3Aclang-tidy)
[![Sanitizer](https://github.com/taocpp/taopq/workflows/Sanitizer/badge.svg)](https://github.com/taocpp/taopq/actions?query=workflow%3ASanitizer)
[![Code Coverage](https://codecov.io/gh/taocpp/taopq/branch/main/graph/badge.svg?token=ykWa8RRdyk)](https://codecov.io/gh/taocpp/taopq)

taoPQ is a lightweight C++ client library for accessing a PostgreSQL database.
It has no dependencies beyond [`libpq`➚](https://www.postgresql.org/docs/current/libpq.html), the C application programmer's interface to PostgreSQL.

## Introduction

The library provides support for database connections, transactions, nested transactions, prepared statements, large objects, connection pools, high-speed bulk data transfer, and more.
An extensible traits mechanism is used to convert C++ types into SQL statement parameters, and conversely to convert query results into arbitrary C++ types.
The following example shows the basic look and feel of the library.

```c++
#include <iostream>
#include <tao/pq.hpp>

int main()
{
   // open a connection to the database
   const auto conn = tao::pq::connection::create( "dbname=template1" );

   // execute statements
   conn->execute( "DROP TABLE IF EXISTS users" );
   conn->execute( "CREATE TABLE users ( name TEXT PRIMARY KEY, age INTEGER NOT NULL )" );

   // prepare statements
   conn->prepare( "insert_user", "INSERT INTO users ( name, age ) VALUES ( $1, $2 )" );

   {
      // begin transaction
      const auto tr = conn->transaction();

      // execute previously prepared statements
      tr->execute( "insert_user", "Daniel", 42 );
      tr->execute( "insert_user", "Tom", 41 );
      tr->execute( "insert_user", "Jerry", 29 );

      // commit transaction
      tr->commit();
   }

   // query data
   const auto users = conn->execute( "SELECT name, age FROM users WHERE age >= $1", 40 );

   // iterate and convert results
   for( const auto& row : users ) {
      std::cout << row[ "name" ].as< std::string >() << " is "
                << row[ "age" ].as< unsigned >() << " years old.\n";
   }
}
```

## Documentation

* [Table of Content](doc/TOC.md)
* [Requirements](doc/Requirements.md)
* [Installation](doc/Installation.md)
* [Getting Started](doc/Getting-Started.md)
* [Connection](doc/Connection.md)
* [Transaction](doc/Transaction.md)
* [Statement](doc/Statement.md)
* [Result](doc/Result.md)

## Contact

<a href="https://discord.gg/VQYkppcgqN"><img align="right" src="https://discordapp.com/api/guilds/790164930083028993/embed.png?style=banner2" alt="Join us on Discord"></a>

For questions and suggestions regarding taoPQ, success or failure stories, and any other kind of feedback, please feel free to join our [Discord➚](https://discord.gg/VQYkppcgqN) server, open a [discussion](https://github.com/taocpp/taopq/discussions), an [issue](https://github.com/taocpp/taopq/issues) or a [pull request](https://github.com/taocpp/taopq/pulls) on GitHub or contact the authors at `taocpp(at)icemx.net`.

## The Art of C++

taoPQ is part of [The Art of C++](https://taocpp.github.io/).

[<img alt="colinh" src="https://avatars.githubusercontent.com/u/113184" width="120">](https://github.com/colinh)
[<img alt="d-frey" src="https://avatars.githubusercontent.com/u/3956325" width="120">](https://github.com/d-frey)
[<img alt="uilianries" src="https://avatars.githubusercontent.com/u/4870173" width="120">](https://github.com/uilianries)

## License

<a href="https://opensource.org/licenses/BSL-1.0"><img align="right" src="https://opensource.org/files/OSIApproved.png" width="150" hspace="20" alt="Open Source Initiative"></a>

Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch

taoPQ is certified [Open Source➚](http://www.opensource.org/docs/definition.html) software.
It is [licensed➚](https://pdimov.github.io/blog/2020/09/06/why-use-the-boost-license/) under the terms of the [Boost Software License, Version 1.0➚](https://www.boost.org/LICENSE_1_0.txt) reproduced here.

> Boost Software License - Version 1.0 - August 17th, 2003
>
> Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute, and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the Software is furnished to do so, all subject to the following:
>
> The copyright notices in the Software and this entire statement, including the above license grant, this restriction and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all derivative works of the Software, unless such copies or derivative works are solely in the form of machine-executable object code generated by a source language processor.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

[The Art of C++]: https://taocpp.github.io/
