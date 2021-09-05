# Parameter Type Conversion

When executing statements, you can pass any number of parameters after the statement itself to the `execute()`-method.
Each parameter then gets converted through the parameter traits class template into one or more positional parameters for the SQL statement.
By default, the following C++ types are available for use as statement parameters.

## NULL

If you want to pass NULL to the database, you pass `tao::pq::null`.

## Fundamental Types

* Booleans
  * `bool`
* Character
  * `char`
* Integral Types
  * `signed char` (8-bit signed integer)
  * `unsigned char` (8-bit unsigned integer)
  * `short`
  * `unsigned short`
  * `int`
  * `unsigned int`
  * `long`
  * `unsigned long`
  * `long long`
  * `unsigned long long`
* Floating Point Types
  * `float`
  * `double`
  * `long double`
* Strings
  * `const char*`
  * `std::string`
  * `std::string_view`
* [Binary](Binary-Data.md) ([`BYTEA`](https://www.postgresql.org/docs/current/datatype-binary.html))
  * `std::basic_string< unsigned char >`
  * `std::basic_string< std::byte >`
  * `std::basic_string_view< unsigned char >`
  * `std::basic_string_view< std::byte >`
* [Arrays](Arrays.md) ([`ARRAY`](https://www.postgresql.org/docs/current/arrays.html))
  * `std::array< T, N >`
  * `std::list< T >`
  * `std::set< T >`
  * `std::unordered_set< T >`
  * `std::vector< T >`

## `std::optional< T >`

Represents a NULLable type.

If the optional is not empty, then the parameters for `T` are generated.
If the optional is empty, it is equivalent to one or more `tao::pq::null` parameter(s).
The number of NULL values generated depends on the number of parameters that `T` would generate.

## `std::pair< T, U >`

Generates all parameters from `T`, then all parameters from `U`, in that order.
Note that this generates at least two parameters, possibly more.
Pairs can be nested, e.g. `std::pair< std::pair< int, int >, int >` would generate three parameters.

## `std::tuple< Ts... >`

As a generalisation of pairs, tuples generate all parameters for their individual element, in order.

## Custom Data Types

TODO

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
