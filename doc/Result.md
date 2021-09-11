# Result

Result sets can be iterated or conveniently converted into a C++ data structure.
Predefined types include most STL containers, `std::pair`/`std::tuple`, and `std::optional` for [nullable](https://en.wikipedia.org/wiki/Nullable_type) values.
Again custom types can be added with custom conversion functions.

## Synopsis

Don't be intimidated by the size of the API, as you can see in the latter parts several methods are just single-line convenience forwarders.

```c++
namespace tao::pq
{
   namespace internal
   {
      class zsv;  // zero-terminated string view
   }

   class row;

   class result final
   {
   private:
      // satisfies LegacyRandomAccessIterator, see
      // https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
      class const_iterator;

   public:
      bool has_rows_affected() const noexcept;

      auto rows_affected() const
         -> std::size_t;

      auto columns() const noexcept
         -> std::size_t;

      auto name( const std::size_t column ) const
         -> std::string;

      auto index( const internal::zsv in_name ) const
         -> std::size_t;

      bool empty() const;

      auto size() const
         -> std::size_t;

      auto begin() const
         -> const_iterator;

      auto end() const
         -> const_iterator;

      bool is_null( const std::size_t row, const std::size_t column ) const;

      auto get( const std::size_t row, const std::size_t column ) const
         -> const char*;

      auto operator[]( const std::size_t row ) const noexcept
         -> pq::row;

      auto at( const std::size_t row ) const
         -> pq::row;

      // expects size()==1, converts the only row to T
      template< typename T >
      auto as() const
         -> T;

      template< typename T >
      auto optional() const
         -> std::optional< T >;

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
      auto as_container() const
         -> T;

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
      auto underlying_raw_ptr() noexcept
         -> PGresult*;

      auto underlying_raw_ptr() const noexcept
         -> const PGresult*;
   };
}
```

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
