# Bulk Transfer

**TODO**

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

   class transaction;

   class table_writer final
   {
   public:
      template< typename... As >
      table_writer( const std::shared_ptr< transaction >& transaction, const internal::zsv statement, As&&... as );

      ~table_writer();

      table_writer( const table_writer& ) = delete;
      table_writer( table_writer&& ) = delete;
      void operator=( const table_writer& ) = delete;
      void operator=( table_writer&& ) = delete;

      void insert_raw( const std::string_view data );

      template< typename... As >
      void insert( As&&... as );

      auto commit() -> std::size_t;
   };

   using null_t = decltype( null );

   class table_row;
   class table_field;

   class table_reader final
   {
   private:
      class const_iterator;

   public:
      template< typename... As >
      table_reader( const std::shared_ptr< transaction >& transaction, const internal::zsv statement, As&&... as );

      ~table_reader() = default;

      table_reader( const table_reader& ) = delete;
      table_reader( table_reader&& ) = delete;
      void operator=( const table_reader& ) = delete;
      void operator=( table_reader&& ) = delete;

      auto columns() const noexcept -> std::size_t;

      auto get_raw_data() -> std::string_view;
      bool parse_data() noexcept;

      bool get_row();
      bool has_data() const noexcept;

      auto raw_data() const noexcept
         -> const std::vector< const char* >&;

      auto row() noexcept -> table_row;

      auto begin() -> const_iterator;
      auto end() noexcept -> const_iterator;

      auto cbegin() -> const_iterator;
      auto cend() noexcept -> const_iterator;

      template< typename T >
      auto as_container() -> T;

      // convenience conversions to standard containers
      template< typename... Ts >
      auto vector()
      {
         return as_container< std::vector< Ts... > >();
      }

      template< typename... Ts >
      auto list()
      {
         return as_container< std::list< Ts... > >();
      }

      template< typename... Ts >
      auto set()
      {
         return as_container< std::set< Ts... > >();
      }

      template< typename... Ts >
      auto multiset()
      {
         return as_container< std::multiset< Ts... > >();
      }

      template< typename... Ts >
      auto unordered_set()
      {
         return as_container< std::unordered_set< Ts... > >();
      }

      template< typename... Ts >
      auto unordered_multiset()
      {
         return as_container< std::unordered_multiset< Ts... > >();
      }

      template< typename... Ts >
      auto map()
      {
         return as_container< std::map< Ts... > >();
      }

      template< typename... Ts >
      auto multimap()
      {
         return as_container< std::multimap< Ts... > >();
      }

      template< typename... Ts >
      auto unordered_map()
      {
         return as_container< std::unordered_map< Ts... > >();
      }

      template< typename... Ts >
      auto unordered_multimap()
      {
         return as_container< std::unordered_multimap< Ts... > >();
      }
   };

   class table_row
   {
   private:
      // satisfies LegacyRandomAccessIterator, see
      // https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
      class const_iterator;

   public:
      auto slice( const std::size_t offset, const std::size_t in_columns ) const -> table_row;

      auto columns() const noexcept -> std::size_t;

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

      auto at( const std::size_t column ) const -> table_field;
      auto operator[]( const std::size_t column ) const noexcept -> table_field;

      friend void swap( table_row& lhs, table_row& rhs ) noexcept;
   };

   class table_field
   {
   public:
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

   bool operator==( const table_field& f, null_t )
   {
      return f.is_null();
   }

   bool operator==( null_t, const table_field& f )
   {
      return f.is_null();
   }

   bool operator!=( const table_field& f, null_t )
   {
      return !f.is_null();
   }

   bool operator!=( null_t, const table_field& f )
   {
      return !f.is_null();
   }
}
```

**TODO**

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2023 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
