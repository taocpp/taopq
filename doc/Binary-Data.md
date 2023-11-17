# Binary Data

PostgreSQL stores binary data either as a field with the [`BYTEA`➚](https://www.postgresql.org/docs/current/datatype-binary.html) data type or as a [large object➚](https://www.postgresql.org/docs/current/largeobjects.html).

Large Objects in taoPQ have their own representation discussed in the [Large Object](Large-Object.md) chapter.

## The [`BYTEA`➚](https://www.postgresql.org/docs/current/datatype-binary.html) Data Type

In PostgreSQL strings are a sequence of bytes that is valid in a given encoding.
One or more bytes can represent individual characters or code points.
This means that not all sequences of bytes are a valid string and therefore binary data can generally not be represented by PostgreSQL as a string.

Binary data is different from strings, as binary data is a collection of an arbitrary sequence of bytes.
Any byte is treated independently of its surrounding bytes and can have any value, including '\0'.
It is therefore crucial to represent binary data with the dedicated [`BYTEA`➚](https://www.postgresql.org/docs/current/datatype-binary.html) data type.

## C++ Binary Data

The individual bytes are represented by [`std::byte`➚](https://en.cppreference.com/w/cpp/types/byte).
As there is no one-size-fits-all data type to handle binary data in C++, we allow several options.

We mostly represent binary data with `tao::pq::binary` and `tao::pq::binary_view`.
These are type aliases for [`std::basic_string<std::byte>`➚](https://en.cppreference.com/w/cpp/string/basic_string) and [`std::basic_string_view<std::byte>`➚](https://en.cppreference.com/w/cpp/string/basic_string_view), respectively.
Other types are supported as well, specifically `std::basic_string<unsigned char>` and `std::basic_string_view<unsigned char>`.

## Passing Binary Data

When you pass binary data to taoPQ, we only require a view to be passed.
As a view is a non-owning data type, constructing an instance of it is cheap.

If you have other data types like [`std::vector<std::byte>`➚](https://en.cppreference.com/w/cpp/container/vector) or [`std::span<std::byte>`➚](https://en.cppreference.com/w/cpp/container/span) (C++20), you can create a binary data view by using

```c++
template< typename T >
auto tao::pq::to_binary_view( const T* data, const std::size_t size ) noexcept -> tao::pq::binary_view;

template< typename T >
auto tao::pq::to_binary_view( const T& data ) noexcept
{
   return pq::to_binary_view( std::data( data ), std::size( data ) );
}
```

The former function requires (and checks) that `T` has a size of 1 byte.
If you want to store larger `T`s as binary data you need to manually convert the pointer and size appropriately.

The second method requires the data type `T` to be a suitable candidate for [`std::data()`➚](https://en.cppreference.com/w/cpp/iterator/data) and [`std::size()`➚](https://en.cppreference.com/w/cpp/iterator/size), which requires the data to be stored in a contiguous block of memory.
We do not offer any convenience methods to create binary data from distributed data structures, i.e. `std::list<std::byte>` is not supported.

## Receiving Binary Data

When receiving binary data, a non-owning view is insufficient, hence we return `tao::pq::binary`.
In some cases other alternatives are offered, i.e. you may provide a buffer that the data is written to.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2023 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
