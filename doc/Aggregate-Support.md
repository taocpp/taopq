# Aggregate Support

taoPQ allows the direct use of "simple" aggregates as parameters and result types.

## Requirements

An aggregate data type `T` suitable for taoPQ support must meet the following requirements:

* [`std::is_aggregate_v< T >`➚](https://en.cppreference.com/w/cpp/types/is_aggregate) must yield `true`.
* [`std::is_empty_v< T >`➚](https://en.cppreference.com/w/cpp/types/is_empty) must yield `false`.
* [`std::is_union_v< T >`➚](https://en.cppreference.com/w/cpp/types/is_union) must yield `false`.
* `T` must not have any base classes.
* `T` must not have more than 99 member variables. (this limit can be raised, open an [issue](https://github.com/taocpp/taopq/issues) if necessary)

## Registration

We currently require explicit registration of aggregate data types.
This is done as a precaution, and to avoid conflicts with other data types.

A data type `T` can be registered as both a parameter and a result type by specializing `tao::pq::is_aggregate` as follows:

```c++
template<>
inline constexpr bool tao::pq::is_aggregate< T > = true;
```

If necessary, you can control independently whether a data type is suitable as a parameter or a result type by specializing `tao::pq::is_aggregate_parameter` and `tao::pq::is_aggregate_result` as follows:

```c++
template<>
inline constexpr bool tao::pq::is_aggregate_parameter< T > = true;

template<>
inline constexpr bool tao::pq::is_aggregate_result< T > = true;
```

## Direct Result Conversion

An aggregate data type `T` that is registered as a result type enables the direct conversion of a `tao::pq::row` to `T`.
This is convenient when retrieving results and iterating over them, it enables you to write:

```c++
const auto result = conn->execute( "SELECT ... FROM ..." ); // some query
for( const T t : result ) {
   // use t
}
```

:interrobang: Compilers might complain about extra copies and that you should use `const T&`, but that would not work and there are no extra copies.

## Example

The following is a short, but complete example of how to use aggregates with taoPQ:

```c++
#include <iostream>
#include <tao/pq.hpp>

// an aggregate
struct user
{
   std::string name;
   unsigned age;
   std::string planet;
};

template<>
inline constexpr bool tao::pq::is_aggregate< user > = true;

int main()
{
   // open a connection to the database
   const auto conn = tao::pq::connection::create( "dbname=template1" );

   // execute statements
   conn->execute( "DROP TABLE IF EXISTS users" );
   conn->execute( "CREATE TABLE users ( name TEXT PRIMARY KEY, age INTEGER NOT NULL, planet TEXT NOT NULL )" );

   // prepare statements
   conn->prepare( "insert_user", "INSERT INTO users ( name, age, planet ) VALUES ( $1, $2, $3 )" );

   // execute previously prepared statements
   conn->execute( "insert_user", user{ "R. Daneel Olivaw", 19230, "Earth" } );
   conn->execute( "insert_user", user{ "R. Giskard Reventlov", 42, "Aurora" } );

   // query and convert data
   for( const user u : conn->execute( "SELECT name, age, planet FROM users" ) ) {
      std::cout << u.name << " from " << u.planet << " was " << u.age << " years old.\n";
   }
}
```

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
