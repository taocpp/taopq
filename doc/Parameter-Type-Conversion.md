# Parameter Type Conversion

When [executing statements](Statement.md), you can pass any number of parameters after the statement itself to the `execute()`-method.
Each parameter then gets converted through the parameter traits class template into one or more positional parameters for the SQL statement.
By default, the following C++ types are available for use as parameters.

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
* [Binary](Binary-Data.md) ([`BYTEA`➚](https://www.postgresql.org/docs/current/datatype-binary.html))
  * `std::basic_string< unsigned char >`
  * `std::basic_string< std::byte >`
  * `std::basic_string_view< unsigned char >`
  * `std::basic_string_view< std::byte >`
* [Arrays](Arrays.md) ([`ARRAY`➚](https://www.postgresql.org/docs/current/arrays.html))
  * `std::array< T, N >`
  * `std::list< T >`
  * `std::set< T >`
  * `std::unordered_set< T >`
  * `std::vector< T >`

## `std::optional< T >`

Represents a [nullable➚](https://en.wikipedia.org/wiki/Nullable_type) type.

If the optional is not empty, then the parameters from `T` are generated.
If the optional is empty, it is equivalent to one or more `tao::pq::null` parameter(s).
The number of NULL values generated depends on the number of parameters that `T` would generate.

## `std::pair< T, U >`

Generates all parameters from `T`, then all parameters from `U`, in that order.
Note that this generates at least two parameters, possibly more.
Pairs can be nested, e.g. `std::pair<std::pair<int,int>,int>` would generate three parameters.

## `std::tuple< Ts... >`

As a generalisation of pairs, tuples generate all parameters for their individual element, in order.

## Custom Data Types

Custom data types can be registered in two different ways.

### `to_taopq_param()`

You can add a method called `to_taopq_param()` to your class, any value returned will then be fed into the parameters as outlined above.
Usually, that means a simple conversion will return a single known type, more complicated types return a `std::tuple` to return multiple parameters for the SQL statement.
Let's look at two examples, starting with the simplest case first:

```c++
class my_int_wrapper
{
private:
   int value;

public:
   explicit my_int_wrapper( int v ) : value( v ) {}

   auto to_taopq_param() const noexcept
   {
      return value;
   }
};
```

You can now pass values of type `my_int_wrapper` as parameters to call taoPQ's `execute()`-methods.

If your class has more members, you can return multiple values:

```c++
class my_coordinates
{
private:
   double x,y,z;

public:
   //  ctors, etc.

   auto to_taopq_param() const noexcept
   {
      return std::tie( x, y, z );
   }
};
```

The above means that each time you pass a `my_coordinates` instance as a parameter to an `execute()`-method, three positional parameters are added and can be referenced from the SQL statement.

If you can't modify the class you could provide a free function called `to_taopq_param` instead.
That function must accept a single parameter of the class you want to register.
Example:

```c++
struct some_coordinates
{
   double x,y,z;
};

auto to_taopq_param( const some_coordinates& v ) noexcept
{
   return std::tie( v.x, v.y, v.z );
}
```

The free function is found either by [ADL➚](https://en.cppreference.com/w/cpp/language/adl) or in namespace `tao::pq`.

Note that any returned value in the above examples can itself be a registered custom type.
taoPQ will simply expand parameters recursively.

### `tao::pq::parameter_traits< T >`

If the above custom data type registration via `to_taopq_param()` is somehow not sufficient, you can specialize the `tao::pq::parameter_traits` class template.
For now please consult the source code or ask the developers.
TODO: Write proper documentation.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
