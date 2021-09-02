// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TABLE_READER_HPP
#define TAO_PQ_TABLE_READER_HPP

#include <cassert>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <libpq-fe.h>

#include <tao/pq/result.hpp>
#include <tao/pq/table_row.hpp>

namespace tao::pq
{
   class transaction;

   class table_reader final
   {
   protected:
      std::shared_ptr< transaction > m_previous;
      std::shared_ptr< transaction > m_transaction;
      const result m_result;
      std::unique_ptr< char, decltype( &PQfreemem ) > m_buffer;
      std::vector< const char* > m_data;

   public:
      table_reader( const std::shared_ptr< transaction >& transaction, const std::string& statement );
      ~table_reader() = default;

      table_reader( const table_reader& ) = delete;
      table_reader( table_reader&& ) = delete;
      void operator=( const table_reader& ) = delete;
      void operator=( table_reader&& ) = delete;

      [[nodiscard]] auto columns() const noexcept -> std::size_t
      {
         return m_result.columns();
      }

      // note: the following API is experimental and subject to change

      [[nodiscard]] auto get_raw_data() -> std::string_view;
      [[nodiscard]] auto parse_data() noexcept -> bool;

      [[nodiscard]] auto get_row() -> bool
      {
         (void)get_raw_data();
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
         return table_row( *this, 0, columns() );
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
         [[nodiscard]] friend auto operator!=( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_columns != rhs.m_columns;
         }

         auto operator++() noexcept -> const_iterator&
         {
            if( !m_reader.get_row() ) {
               m_columns = 0;
            }
            return *this;
         }

         [[nodiscard]] auto operator*() const noexcept -> const table_row&
         {
            return *this;
         }
      };

   public:
      [[nodiscard]] auto begin() -> const_iterator;
      [[nodiscard]] auto end() noexcept -> const_iterator;

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
