# Result

When [executing statements](Statement.md) you receive a result object.
A result comes in two flavours, depending on what statement was executed.

When executing a [query statement➚](https://www.postgresql.org/docs/current/queries-overview.html), the database returns a result set, i.e. any number of rows containing one or more fields of data.

When executing non-query statements, you can usually only extract the number of affected rows.

Query results can be iterated or conveniently converted into a C++ data structure.
Predefined types include most arithmetic C++ data types, STL containers, `std::pair`/`std::tuple`, and `std::optional` for [nullable➚](https://en.wikipedia.org/wiki/Nullable_type) values.
Again custom types can be added with custom conversion functions.

## Synopsis

Don't be intimidated by the size of the API, as you can see several methods are just single-line convenience forwarders.

We will first give the synopsis of everything, afterwards we will break down the API into small logical portions.

```c++
namespace tao::pq
{
   namespace internal
   {
      class zsv;  // zero-terminated string view
   }

   using null_t = decltype( null );

   class row;
   class field;

   class result final
   {
   private:
      // satisfies LegacyRandomAccessIterator, see
      // https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
      class const_iterator;

   public:
      // non-query result access
      bool has_rows_affected() const noexcept;
      auto rows_affected() const -> std::size_t;

      // information about the returned fields
      auto columns() const noexcept -> std::size_t;

      auto name( const std::size_t column ) const -> std::string;
      auto index( const internal::zsv in_name ) const -> std::size_t;

      // size of the result set
      bool empty() const;
      auto size() const -> std::size_t;

      // iteration
      auto begin() const -> const_iterator;
      auto end() const -> const_iterator;

      auto cbegin() const -> const_iterator;
      auto cend() const -> const_iterator;

      // get basic information about a field
      bool is_null( const std::size_t row, const std::size_t column ) const;
      auto get( const std::size_t row, const std::size_t column ) const -> const char*;

      // access rows
      auto operator[]( const std::size_t row ) const noexcept -> pq::row;
      auto at( const std::size_t row ) const -> pq::row;

      // convenience conversions for whole result sets

      // expects size()==1, converts the only row to T
      template< typename T >
      auto as() const -> T;

      template< typename T >
      auto optional() const -> std::optional< T >;

      // convenience conversions to pair/tuple
      template< typename T, typename U >
      auto pair() const
      {
         return as< std::pair< T, U > >();
      }

      template< typename... Ts >
      auto tuple() const
      {
         return as< std::tuple< Ts... > >();
      }

      // convert each row into T::value_type and add to a container of type T
      template< typename T >
      auto as_container() const -> T;

      // convenience conversions to standard containers
      template< typename... Ts >
      auto vector() const
      {
         return as_container< std::vector< Ts... > >();
      }

      template< typename... Ts >
      auto list() const
      {
         return as_container< std::list< Ts... > >();
      }

      template< typename... Ts >
      auto set() const
      {
         return as_container< std::set< Ts... > >();
      }

      template< typename... Ts >
      auto multiset() const
      {
         return as_container< std::multiset< Ts... > >();
      }

      template< typename... Ts >
      auto unordered_set() const
      {
         return as_container< std::unordered_set< Ts... > >();
      }

      template< typename... Ts >
      auto unordered_multiset() const
      {
         return as_container< std::unordered_multiset< Ts... > >();
      }

      template< typename... Ts >
      auto map() const
      {
         return as_container< std::map< Ts... > >();
      }

      template< typename... Ts >
      auto multimap() const
      {
         return as_container< std::multimap< Ts... > >();
      }

      template< typename... Ts >
      auto unordered_map() const
      {
         return as_container< std::unordered_map< Ts... > >();
      }

      template< typename... Ts >
      auto unordered_multimap() const
      {
         return as_container< std::unordered_multimap< Ts... > >();
      }

      // access underlying result pointer from libpq
      auto underlying_raw_ptr() noexcept -> PGresult*;
      auto underlying_raw_ptr() const noexcept -> const PGresult*;
   };

   class row
   {
   private:
      // satisfies LegacyRandomAccessIterator, see
      // https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
      class const_iterator;

   public:
      auto slice( const std::size_t offset, const std::size_t in_columns ) const -> row;

      auto columns() const noexcept -> std::size_t;

      auto name( const std::size_t column ) const -> std::string;
      auto index( const internal::zsv in_name ) const -> std::size_t;

      // iteration
      auto begin() const -> const_iterator;
      auto end() const -> const_iterator;

      auto cbegin() const -> const_iterator;
      auto cend() const -> const_iterator;

      bool is_null( const std::size_t column ) const;
      auto get( const std::size_t column ) const -> const char*;

      template< typename T >
      auto get( const std::size_t column ) const -> T;

      template< typename T >
      auto optional( const std::size_t column ) const
      {
         return get< std::optional< T > >( column );
      }

      template< typename T >
      auto as() const -> T;

      template< typename T >
      auto optional() const
      {
         return as< std::optional< T > >();
      }

      template< typename T, typename U >
      auto pair() const
      {
         return as< std::pair< T, U > >();
      }

      template< typename... Ts >
      auto tuple() const
      {
         return as< std::tuple< Ts... > >();
      }

      auto at( const std::size_t column ) const -> field;
      auto operator[]( const std::size_t column ) const noexcept -> field;

      auto at( const internal::zsv in_name ) const -> field;
      auto operator[]( const internal::zsv in_name ) const -> field;

      friend void swap( row& lhs, row& rhs ) noexcept;
   };

   class field
   {
   public:
      auto name() const -> std::string;
      auto index() const -> std::size_t;

      bool is_null() const;
      auto get() const -> const char*;

      template< typename T >
      auto as() const -> T;

      template< typename T >
      auto optional() const
      {
         return as< std::optional< T > >();
      }
   };

   bool operator==( const field& f, null_t )
   {
      return f.is_null();
   }

   bool operator==( null_t, const field& f )
   {
      return f.is_null();
   }

   bool operator!=( const field& f, null_t )
   {
      return !f.is_null();
   }

   bool operator!=( null_t, const field& f )
   {
      return !f.is_null();
   }
}
```

## Non-Query Results

For non-query results, i.e. when you called an `INSERT`-, `UPDATE`-, or `DELETE`-statement, you really only need the `rows_affected()`-method.
In generic programming, when you might not know what kind of result you have, you can check whether or not a result is a non-query result by calling the `has_rows_affected()`-method.

```c++
bool tao::pq::result::has_rows_affected() const;
auto tao::pq::result::rows_affected() const -> std::size_t;
```

## Query Results

[Query results➚](https://www.postgresql.org/docs/current/queries-overview.html) are non-mutable data sets, they are cheap to copy, move, or assign and you can iterate over the data multiple times in random order.
Likewise, rows are also non-mutable, as well as fields.
This also means iterators will behave as constant iterators.

Query results act similar to a random-access container.
The don't fully implement the [container requirements➚](https://en.cppreference.com/w/cpp/named_req/Container), but a reasonable subset of those are provided.

Rows and fields are non-owning, meaning they are only valid as long as the query result instance is still valid.

### Basics

You can query the container's size, i.e. the number of rows it contains, by calling the `size()`-method.
The `empty()`-method will, of course, return whether the size of the container is zero or not.

```c++
bool tao::pq::result::empty() const;
auto tao::pq::result::size() const -> std::size_t;
```

The number of columns, column order, and the column name is the same for all rows of a result set.
You can query the number of columns by calling the `columns()`-method.
You can retrieve the name of a column using the `name()`-method, or the column index by using the `index()`-method.

```c++
auto tao::pq::result::columns() const -> std::size_t;
auto tao::pq::result::name( std::size_t column ) const -> std::string;
auto tao::pq::result::index( tao::pq::internal::zsv name ) const -> std::size_t;
```

Direct access to the data is provided by the `is_null()`- and the `get()`-methods.
The latter returns the raw string as returned by `libpq`, it is a low level access method that is rarely used directly.

```c++
bool tao::pq::result::is_null( std::size_t row, std::size_t column ) const;
auto tao::pq::result::get( std::size_t row, std::size_t column ) const -> const char*;
```

### Row Access

You can iterate over the container's elements, the rows, with the usual methods.
This is what the `begin()`- and `end()`-methods are for, also allowing for the convenient use of [range-based for loops➚](https://en.cppreference.com/w/cpp/language/range-for).

```c++
auto tao::pq::result::begin() const -> tao::pq::result::const_iterator;
auto tao::pq::result::end() const -> tao::pq::result::const_iterator;
```

The identical `cbegin()`- and `cend()`-methods are provided for completeness.

Here's an example of how to iterate all rows:

```c++
const tao::pq::result result = ...;
for( const auto& row : result ) {
   // use row to access your data
}
```

or more traditionally:

```c++
const tao::pq::result result = ...;
for( auto it = std::begin( result ); it != std::end( result ); ++it ) {
   // use *it to access your row's data
}
```

Alternatively, you can use an index to access the rows.

```c++
const tao::pq::result result = ...;
for( std::size_t i = 0; i < result.size(); ++i ) {
   // use result[ i ] or result.at( i ) to access your row's data
}
```

This is enabled by the accessors, the `at()`-method and the `[]`-operator.

```c++
auto tao::pq::result::at( std::size_t index ) const -> tao::pq::row;
auto tao::pq::result::operator[]( std::size_t index ) const noexcept -> tao::pq::row;
```

More conversion methods will be discussed later, after we covered the basics for rows and fields.

### Field Access

Given a row, you can query information about the fields with the same methods as for the result itself.

```c++
auto tao::pq::row::columns() const -> std::size_t;
auto tao::pq::row::name( std::size_t column ) const -> std::string;
auto tao::pq::row::index( tao::pq::internal::zsv name ) const -> std::size_t;
```

Direct access to the data is provided by the `is_null()`- and the `get()`-methods.
The latter returns the raw string as returned by `libpq`, it is a low level access method that is rarely used directly.

```c++
bool tao::pq::row::is_null( std::size_t column ) const;
auto tao::pq::row::get( std::size_t column ) const -> const char*;
```

You can iterate over the row's elements, the fields, with the usual methods.
This is what the `begin()`- and `end()`-methods are for, also allowing for the convenient use of [range-based for loops➚](https://en.cppreference.com/w/cpp/language/range-for).

```c++
auto tao::pq::row::begin() const -> tao::pq::row::const_iterator;
auto tao::pq::row::end() const -> tao::pq::row::const_iterator;
```

The identical `cbegin()`- and `cend()`-methods are provided for completeness.

Here's an example of how to iterate all fields:

```c++
const tao::pq::result result = ...;
for( const auto& row : result ) {
   for( const auto& field : row ) {
      // use field to access your data
   }
}
```

or more traditionally:

```c++
const tao::pq::result result = ...;
for( auto it = std::begin( result ); it != std::end( result ); ++it ) {
   for( auto jt = std::begin( *it ); jt != std::end( *it ); ++jt ) {
      // use *jt to access your fields's data
   }
}
```

Alternatively, you can use an index to access the fields.

```c++
const tao::pq::result result = ...;
for( std::size_t i = 0; i < result.size(); ++i ) {
   for( std::size_t j = 0; j < result[ i ].columns(); ++j ) {
      // use result[ i ][ j ] or result.at( i ).at( j ) to access your field's data
   }
}
```

This is enabled by the accessors, the `at()`-method and the `[]`-operator.

```c++
auto tao::pq::row::at( std::size_t index ) const -> tao::pq::field;
auto tao::pq::row::operator[]( std::size_t index ) const noexcept -> tao::pq::field;
```

More conversion methods will be discussed later, after we covered the basics for fields.

### Fields

You can query a field's name by calling the `name()`-method.

```c++
auto tao::pq::field::name() const -> std::string;
```

Direct access to the data is provided by the `is_null()`- and the `get()`-methods.
The latter returns the raw string as returned by `libpq`, it is a low level access method that is rarely used directly.

```c++
bool tao::pq::field::is_null() const;
auto tao::pq::field::get() const -> const char*;
```

Now that we covered the basics, we can retrieve the actual data and convert it to the data types we need.

## Field Data Conversion

A field can be converted to any data type `T` that is a single field wide.
What we mean by that is, that `tao::pq::result_traits_size< T >` yields 1.
This is the case for `const char*`, `std::string`, `int`, etc.

In order to convert a field to the data type you want, you can use the `as()`-method.

```c++
template< typename T >
auto tao::pq::field::as() const -> T;
```

The conversion is handled by the `tao::pq::result_traits` class template, which is documented in the [result type conversion](Result-Type-Conversion.md) chapter.

A field also has a convenience method to convert directly into a `std::optional<T>`.

```c++
template< typename T >
auto tao::pq::field::optional() const
{
   return as< std::optional< T > >();
}
```

## Row Data Conversion

**TODO** Finish this up for rows and results...

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
