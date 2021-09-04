# Large Object

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
      static auto create( const std::shared_ptr< transaction >& transaction, const oid desired_id = oid::invalid )
         -> oid;

      static void remove( const std::shared_ptr< transaction >& transaction, const oid id );

      static auto import_file( const std::shared_ptr< transaction >& transaction, const char* filename, const oid desired_id = oid::invalid )
         -> oid;
      static void export_file( const std::shared_ptr< transaction >& transaction, const oid id, const char* filename );

      large_object( const std::shared_ptr< transaction >& transaction, const oid id, const std::ios_base::openmode m );

      large_object( const large_object& ) = delete;
      large_object( large_object&& other ) noexcept;

      ~large_object();

      void operator=( const large_object& ) = delete;
      auto operator=( large_object&& rhs )
         -> large_object&;

      void close();

      auto read( char* data, const std::size_t size )
         -> std::size_t;
      auto read( unsigned char* data, const std::size_t size )
         -> std::size_t;
      auto read( std::byte* data, const std::size_t size )
         -> std::size_t;

      void write( const char* data, const std::size_t size );
      void write( const unsigned char* data, const std::size_t size );
      void write( const std::byte* data, const std::size_t size );

      void write( const char* data );

      template< typename T = binary >
      auto read( const std::size_t size )
         -> T;

      template< typename... Ts >
      void write( Ts&&... ts );

      void resize( const std::int64_t size );

      void seek( const std::int64_t offset, const std::ios_base::seekdir whence );

      auto tell() const
         -> std::int64_t;
   };
}
```

** TODO**

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
