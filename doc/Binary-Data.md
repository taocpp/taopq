# Binary Data

PostgreSQL stores binary data either as a field with the [`BYTEA`➚](https://www.postgresql.org/docs/current/datatype-binary.html) data type or as a [large object➚](https://www.postgresql.org/docs/current/largeobjects.html).

Large Objects in taoPQ have their own representation discussed in the [Large Object](Large-Object.md) chapter.

## The [`BYTEA`➚](https://www.postgresql.org/docs/current/datatype-binary.html) Data Type

In PostgreSQL strings are a sequence of bytes that is valid in a given encoding.
One or more bytes can represent individual characters or code points.
Not all sequences of bytes form a valid string, which means that binary data in general cannot be represented as a string.

Binary data is distinct from strings, as binary data is a collection of an arbitrary sequence of bytes.
Any byte is treated independently of its surrounding bytes and can have any value, including '\0'.
It is therefore crucial to represent binary data with the dedicated [`BYTEA`➚](https://www.postgresql.org/docs/current/datatype-binary.html) data type.

## C++ Binary Data

The individual bytes are represented by [`std::byte`➚](https://en.cppreference.com/w/cpp/types/byte).
As no single C++ data type suits all cases for binary data, we support several options.

We mostly represent binary data with `tao::pq::binary` and `tao::pq::binary_view`.
These are type aliases for [`std::vector<std::byte>`➚](https://en.cppreference.com/w/cpp/container/vector) and [`std::span<std::byte>`➚](https://en.cppreference.com/w/cpp/container/span), respectively.

## Passing Binary Data

When you pass binary data to taoPQ, we only require a view to be passed.
As a view is a non-owning data type, constructing an instance of it is cheap.

If you need to pass other data types, you can create a binary view using

```c++
auto tao::pq::to_binary_view( const auto* data, const std::size_t size ) noexcept -> tao::pq::binary_view;

auto tao::pq::to_binary_view( const auto& data ) noexcept
{
   return pq::to_binary_view( std::data( data ), std::size( data ) );
}
```

The first function requires (and checks) that `T` is exactly one byte in size.
If you want to store larger `T`s as binary data, you need to manually convert the pointer and size appropriately.

The second method requires the data type `T` to be a suitable candidate for [`std::data()`➚](https://en.cppreference.com/w/cpp/iterator/data) and [`std::size()`➚](https://en.cppreference.com/w/cpp/iterator/size), which requires the data to be stored in a contiguous block of memory.
We do not offer any convenience methods to create binary data from distributed data structures, i.e. `std::list<std::byte>` is not supported.

## Receiving Binary Data

When receiving binary data, a non-owning view is insufficient, so we return `tao::pq::binary` instead.
In some cases, alternatives are offered, e.g. you may provide a buffer that the data is written to.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2026 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
