# Result

Result sets can be iterated or conveniently converted into a C++ data structure.
Predefined types include most STL containers, `std::pair`/`std::tuple`, and `std::optional` for NULL-able values.
Again custom types can be added with custom conversion functions.

## Synopsis

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
      class const_iterator
      {
      public:
         auto operator++() noexcept
            -> const_iterator&;

         auto operator*() const noexcept
            -> const row&;

         friend auto operator!=( const const_iterator& lhs, const const_iterator& rhs ) noexcept
            -> bool;
      };

   public:
      auto has_rows_affected() const noexcept
         -> bool;

      auto rows_affected() const
         -> std::size_t;

      auto columns() const noexcept
         -> std::size_t;

      auto name( const std::size_t column ) const
         -> std::string;

      auto index( const internal::zsv in_name ) const
         -> std::size_t;

      auto empty() const
         -> bool;

      auto size() const
         -> std::size_t;

      auto begin() const
         -> const_iterator;

      auto end() const
         -> const_iterator;

      auto is_null( const std::size_t row, const std::size_t column ) const
         -> bool;

      auto get( const std::size_t row, const std::size_t column ) const
         -> const char*;

      auto operator[]( const std::size_t row ) const noexcept
         -> row;

      auto at( const std::size_t row ) const
         -> pq::row;

      template< typename T >
      auto as() const
         -> T;

      template< typename T >
      auto optional() const
         -> std::optional< T >;

      template< typename T, typename U >
      auto pair() const
         -> std::pair< T, U >;

      template< typename... Ts >
      auto tuple() const
         -> std::tuple< Ts... >;

      template< typename T >
      auto as_container() const
         -> T;

      template< typename... Ts >
      auto vector() const
         -> std::vector< Ts... >;

      template< typename... Ts >
      auto list() const
         -> std::list< Ts... >;

      template< typename... Ts >
      auto set() const
         -> std::set< Ts... >);

      template< typename... Ts >
      auto multiset() const
         -> std::multiset< Ts... >;

      template< typename... Ts >
      auto unordered_set() const
         -> std::unordered_set< Ts... >;

      template< typename... Ts >
      auto unordered_multiset() const
         -> std::unordered_multiset< Ts... >;

      template< typename... Ts >
      auto map() const
         -> std::map< Ts... >;

      template< typename... Ts >
      auto multimap() const
         -> std::multimap< Ts... >;

      template< typename... Ts >
      auto unordered_map() const
         -> std::unordered_map< Ts... >;

      template< typename... Ts >
      auto unordered_multimap() const
         -> std::unordered_multimap< Ts... >;
   };
}
```

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
