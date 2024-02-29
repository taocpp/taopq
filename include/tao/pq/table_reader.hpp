// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_TABLE_READER_HPP
#define TAO_PQ_TABLE_READER_HPP

#include <cassert>
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <libpq-fe.h>

#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/table_row.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   class table_reader final
   {
   protected:
      std::shared_ptr< transaction > m_previous;
      std::shared_ptr< transaction > m_transaction;
      std::size_t m_columns;  // NOLINT(modernize-use-default-member-init)
      std::unique_ptr< char, decltype( &PQfreemem ) > m_buffer;
      std::vector< const char* > m_data;

      void check_result();

   public:
      template< typename... As >
      table_reader( const std::shared_ptr< transaction >& transaction, const internal::zsv statement, As&&... as )
         : m_previous( transaction ),
           m_transaction( std::make_shared< internal::transaction_guard >( transaction->connection() ) ),
           m_columns( 0 ),
           m_buffer( nullptr, &PQfreemem )
      {
         m_transaction->send( statement, std::forward< As >( as )... );
         check_result();
      }

      ~table_reader() = default;

      table_reader( const table_reader& ) = delete;
      table_reader( table_reader&& ) = delete;
      void operator=( const table_reader& ) = delete;
      void operator=( table_reader&& ) = delete;

      [[nodiscard]] auto columns() const noexcept -> std::size_t
      {
         return m_columns;
      }

      // note: the following API is experimental and subject to change

      [[nodiscard]] auto get_raw_data() -> std::string_view;
      [[nodiscard]] auto parse_data() noexcept -> bool;

      [[nodiscard]] auto get_row() -> bool
      {
         std::ignore = get_raw_data();
         return parse_data();
      }

      [[nodiscard]] auto has_data() const noexcept -> bool
      {
         return !m_data.empty();
      }

      [[nodiscard]] auto raw_data() const noexcept -> const std::vector< const char* >&
      {
         return m_data;
      }

      [[nodiscard]] auto row() noexcept -> table_row
      {
         assert( has_data() );
         return { *this, 0, columns() };
      }

   private:
      class const_iterator
         : private table_row
      {
      private:
         friend class table_reader;

         const_iterator( const table_row& r ) noexcept
            : table_row( r )
         {}

      public:
         using difference_type = std::int32_t;
         using value_type = const table_row;
         using pointer = const table_row*;
         using reference = const table_row&;

         auto operator++() noexcept -> const_iterator&
         {
            if( !m_reader->get_row() ) {
               m_columns = 0;
            }
            return *this;
         }

         [[nodiscard]] auto operator*() const noexcept -> const table_row&
         {
            return *this;
         }

         [[nodiscard]] auto operator->() const noexcept -> const table_row*
         {
            return this;
         }

         friend void swap( const_iterator& lhs, const_iterator& rhs ) noexcept
         {
            return swap( static_cast< table_row& >( lhs ), static_cast< table_row& >( rhs ) );
         }

         [[nodiscard]] friend auto operator==( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_columns == rhs.m_columns;
         }

         [[nodiscard]] friend auto operator!=( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_columns != rhs.m_columns;
         }
      };

   public:
      [[nodiscard]] auto begin() -> const_iterator;
      [[nodiscard]] auto end() noexcept -> const_iterator;

      [[nodiscard]] auto cbegin()
      {
         return begin();
      }

      [[nodiscard]] auto cend() noexcept
      {
         return end();
      }

      template< typename T >
      [[nodiscard]] auto as_container() -> T
      {
         T nrv;
         for( const auto& row : *this ) {
            nrv.insert( nrv.end(), row.as< typename T::value_type >() );
         }
         return nrv;
      }

      template< typename... Ts >
      [[nodiscard]] auto vector()
      {
         return as_container< std::vector< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto list()
      {
         return as_container< std::list< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto set()
      {
         return as_container< std::set< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto multiset()
      {
         return as_container< std::multiset< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto unordered_set()
      {
         return as_container< std::unordered_set< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto unordered_multiset()
      {
         return as_container< std::unordered_multiset< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto map()
      {
         return as_container< std::map< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto multimap()
      {
         return as_container< std::multimap< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto unordered_map()
      {
         return as_container< std::unordered_map< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto unordered_multimap()
      {
         return as_container< std::unordered_multimap< Ts... > >();
      }
   };

}  // namespace tao::pq

#endif
