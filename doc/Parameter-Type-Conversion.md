# Parameter Type Conversion

When executing statements, you can pass any number of C++ parameters after the statement itself to the `execute()`-method.
Each C++ parameter then gets converted through the parameter traits class template into one or more positional parameters for the statement.
By default, a lot of C++ types are readily available for use as statement parameters.

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

## Binary Data

We accept the following C++ data types for binary data, i.e. PostgreSQL's `BYTEA` data type:

* `std::basic_string< unsigned char >`
* `std::basic_string< std::byte >`
* `std::basic_string_view< unsigned char >`
* `std::basic_string_view< std::byte >`

More information on how taoPQ handles binary data can be found in the [Binary Data](Binary-Data.md) chapter.

## NULL

If you want to pass NULL to the database, you can use `tao::pq::null` as a parameter.

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

## Arrays

By default, the following C++ data types will generate a PostgreSQL [`ARRAY`](Arrays.md):

* `std::array< T, N >`
* `std::list< T >`
* `std::set< T >`
* `std::unordered_set< T >`
* `std::vector< T >`

Note that an array is a single parameter and `T` must yield exactly one parameter.

More information on how taoPQ handles arrays can be found in the [Arrays](Arrays.md) chapter.

## Custom Data Types

The [Parameter Type Conversion](Parameter-Type-Conversion.md) chapter explains how you can register your own custom data types.
