# Result Type Conversion

Depending on what type of [statement](Statement.md) was executed, you receive a [result](Result.md) containing a query result set.
Those results then offer conversions to C++ data types through various methods, e.g. `tao::pq::row::as<T>()`.
This chapter discusses the data types that are available by default and how to register your own custom data types when needed.

## Fundamental Types

By default, the following C++ types are available for use as result types.

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
  * `std::vector< std::byte >`
* [`ARRAY`➚](https://www.postgresql.org/docs/current/arrays.html)
  * `std::list< T >`
  * `std::set< T >`
  * `std::unordered_set< T >`
  * `std::vector< T >`

## `std::optional< T >`

Represents a [nullable➚](https://en.wikipedia.org/wiki/Nullable_type) type.

If the result field is NULL, an empty optional is returned.
If the result field is not NULL, the value is converted to `T` and returned in the optional.

## `std::pair< T, U >`

Returns a `std::pair< T, U >`, hence convertes two (or more) neighboring fields from the result's row.
It can read more than two fields when `T` or `U` themselves read more than one field, i.e. converting to `std::pair<int,std::pair<int,int>>` will read three fields from the result's row.

## `std::tuple< Ts... >`

As a generalisation of pairs, tuples read all fields for their individual elements, in order.

## Aggregates

Any suitable aggregate data type can be used as a result type when registered with taoPQ.

```c++
struct my_aggregate
{
   std::string name;
   unsigned age;
   std::string address;
   bool is_pet_owner;
};

template<>
inline constexpr bool tao::pq::is_aggregate< my_aggregate > = true;
```

See [Aggregate Support](Aggregate-Support.md) for more information.

## Custom Data Types

Custom data types can be registered in two different ways, by using a `from_taopq()` function or method, or by specializing the `tao::pq::result_traits` class template.

### `from_taopq()`

You can use a function or method called `from_taopq()`, which takes one or more suitable parameters and returns a new instance of your type.
There are multiple places where this function or method can be placed.

#### Intrusive Placement

If you have control over a class type, you can add a static method called `from_taopq()`.
The method can be marked `noexcept` if applicable.

```c++
class my_int_wrapper
{
private:
   int value;

public:
   explicit my_int_wrapper( int v ) : value( v ) {}

   static auto from_taopq( const int v ) noexcept
   {
      return my_int_wrapper( v );
   }
};
```

taoPQ will find the class' method, analyze the methods parameters and convert the result's row accordingly.
The method must have at least one parameter, each parameter will read the required number of fields.

#### Non-Intrusive Placement

If you can't modify the class you could specialize `tao::pq::bind<...>` and place a static `from_taopq()`-method inside the specialization.
The method behaves identical to the intrusive version.

Example for the specialization of `tao::pq::bind<...>`:

```c++
struct some_coordinates
{
   double x,y,z;
};

template<>
struct tao::pq::bind< some_coordinates >
{
   static auto from_taopq( const double x, const double y, const double z ) noexcept
   {
      return some_coordinates{ x, y, z };
   }
};
```

:point_up: Note that unlike [`to_taopq()`](Parameter-Type-Conversion.md), there is no free function version for `from_taopq()` available.
This is due to the fact that the custom data type is not a parameter, but rather the returned value.
The parameter list can therefore be identical for multiple custom data types and this could leads to conflicting overloads.
Also, [ADL➚](https://en.cppreference.com/w/cpp/language/adl) would be unavailable as only the parameters' types are considered for ADL.

### `tao::pq::result_traits< T >`

If the above custom data type registration via `from_taopq()` is somehow not sufficient, you can specialize the `tao::pq::result_traits` class template.
For now please consult the source code or ask the developers.

TODO: Write proper documentation.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2025 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
