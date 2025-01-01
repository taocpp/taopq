# Large Object

PostgreSQL has a [large object➚](https://www.postgresql.org/docs/current/largeobjects.html) facility, which provides stream-style access to user data that is stored in a special large-object structure.
Streaming access is useful when working with data values that are too large to manipulate conveniently as a whole.

## Synopsis

```c++
namespace tao::pq
{
   enum class oid : Oid  // Oid defined by libpq
   {
      invalid = InvalidOid,  // InvalidOid defined by libpq

      // undisclosed additional values
   };

   class transaction;

   class large_object final
   {
   public:
      static auto create( const std::shared_ptr< transaction >& transaction,
                          const oid desired_id = oid::invalid ) -> oid;

      static void remove( const std::shared_ptr< transaction >& transaction,
                          const oid id );

      static auto import_file( const std::shared_ptr< transaction >& transaction,
                               const char* filename,
                               const oid desired_id = oid::invalid ) -> oid;

      static void export_file( const std::shared_ptr< transaction >& transaction,
                               const oid id,
                               const char* filename );

      large_object( const std::shared_ptr< transaction >& transaction,
                    const oid id,
                    const std::ios_base::openmode m );

      large_object( const large_object& ) = delete;
      large_object( large_object&& other ) noexcept;

      ~large_object();

      void operator=( const large_object& ) = delete;
      auto operator=( large_object&& rhs ) -> large_object&;

      void close();

      auto read( char* data, const std::size_t size ) -> std::size_t;
      auto read( std::byte* data, const std::size_t size ) -> std::size_t;

      void write( const char* data, const std::size_t size );
      void write( const std::byte* data, const std::size_t size );

      void write( const char* data );

      template< typename T = binary >
      auto read( const std::size_t size ) -> T;

      template< typename... Ts >
      void write( Ts&&... ts );

      void resize( const std::int64_t size );

      auto seek( const std::int64_t offset, const std::ios_base::seekdir whence ) -> std::int64_t;

      auto tell() const -> std::int64_t;
   };
}
```

:point_up: All large object manipulation using these functions must take place within an SQL transaction block, since large object file descriptors are only valid for the duration of a transaction.

## Creating a Large Object

To [create➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-CREATE) a new large object, call

```c++
static auto tao::pq::large_object::create( const std::shared_ptr<tao::pq::transaction>& transaction,
                                           const tao::pq::oid desired_id = tao::pq::oid::invalid ) -> tao::pq::oid;
```

If no desired oid is given, the server will return the oid that was assigned to the new large object.
If you specify a desired oid and that oid is already used, or if any other error occurs, an exception will be thrown.

## Removing a Large Object

To [remove➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-UNLINK) a large object from the database, call

```c++
static void tao::pq::large_object::remove( const std::shared_ptr<tao::pq::transaction>& transaction, const tao::pq::oid id );
```

## Importing a Large Object

To [import➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-IMPORT) an operating system file as a large object, call

```c++
static auto tao::pq::large_object::import_file( const std::shared_ptr<tao::pq::transaction>& transaction,
                                                const char* filename,
                                                const tao::pq::oid desired_id = tao::pq::oid::invalid ) -> tao::pq::oid;
```

If no desired oid is given, the server will return the oid that was assigned to the new large object.
If you specify a desired oid and that oid is already used, or if any other error occurs, an exception will be thrown.

## Exporting a Large Object

To [export➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-EXPORT) a large object into an operating system file, call

```c++
static void tao::pq::large_object::export_file( const std::shared_ptr<tao::pq::transaction>& transaction,
                                                const tao::pq::oid id,
                                                const char* filename );
```

If an error occurs an exception will be thrown.

## Opening an Existing Large Object

To [open➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-OPEN) an existing large object for reading or writing, call

```c++
tao::pq::large_object::large_object( const std::shared_ptr<tao::pq::transaction>& transaction,
                                     const tao::pq::oid id,
                                     const std::ios_base::openmode m );
```

The mode `m` bits control whether the object is opened for reading (`std::ios_base::in`), writing (`std::ios_base::out`), or both.
If an error occurs an exception will be thrown.

The destructor will take care of [closing➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-CLOSE) the large object descriptor.

## Writing Data to a Large Object

To [write➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-WRITE) data to a large object, several methods are available.

```c++
void tao::pq::large_object::write( const char* data, const std::size_t size );
void tao::pq::large_object::write( const std::byte* data, const std::size_t size );

void tao::pq::large_object::write( const char* data );

template< typename... Ts >
void tao::pq::large_object::write( Ts&&... ts );
```

The first three methods write a chunk of data starting at `data` that is `size` bytes long to the large object.

The fourth method expects a zero-terminated string, which will be written to the large object.

The fifth method template forwards its arguments to a call to [`tao::pq::to_binary_view()`](Binary-Data.md), then writes the binary data to the large object.
This allows all data types that are accepted by `tao::pq::to_binary_view()` to be written seamlessly into large objects.

If an error occurs an exception will be thrown.

## Reading Data from a Large Object

To [read➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-READ) data from a large object, several methods are available.

```c++
auto tao::pq::large_object::read( char* data, const std::size_t size ) -> std::size_t;
auto tao::pq::large_object::read( std::byte* data, const std::size_t size ) -> std::size_t;

template< typename T = tao::pq::binary >
auto tao::pq::large_object::read( const std::size_t size ) -> T;
```

The first three methods read up to `size` bytes from the large object into `data`.
The methods will return the number of bytes actually read; this will be less than `size` if the end of the large object is reached first.

The fourth method will create a new object of type `T` and read up to `size` bytes from the large object.
`T` must be one of the following types:

* `std::string`
* [`tao::pq::binary`](Binary-Data.md) aka `std::vector<std::byte>`

:point_up: Although the `size` parameter of the above methods is declared as `std::size_t`, the methods will reject values larger than `INT_MAX`.
In practice, it's best to transfer data in chunks of at most a few megabytes anyway.

If an error occurs an exception will be thrown.

## Seeking in a Large Object

To change the [current read or write➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-SEEK) location associated with a large object, call

```c++
auto tao::pq::large_object::seek( const std::int64_t offset, const std::ios_base::seekdir whence ) -> std::int64_t;
```

This method moves the current location pointer for the large object to the new location specified by `offset`.
The valid values for `whence` are `std::ios_base::beg` (seek from object start), `std::ios_base::cur` (seek from current position), and `std::ios_base::end` (seek from object end).
The return value is the new location pointer.

If an error occurs an exception will be thrown.

## Obtaining the Seek Position of a Large Object

To [obtain➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-TELL) the current read or write location of a large object, call

```c++
auto tao::pq::large_object::tell() const -> std::int64_t;
```

If an error occurs an exception will be thrown.

## Truncating a Large Object

To [truncate➚](https://www.postgresql.org/docs/current/lo-interfaces.html#LO-TRUNCATE) a large object to a given size, call

```c++
void tao::pq::large_object::resize( const std::int64_t size );
```

If `size` is greater than the large object's current length, the large object is extended to the specified length with null bytes ('\0').

If an error occurs an exception will be thrown.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2025 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
