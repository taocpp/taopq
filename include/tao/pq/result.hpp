// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_HPP
#define TAO_PQ_RESULT_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <libpq-fe.h>

#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/result_status.hpp>
#include <tao/pq/row.hpp>

namespace tao::pq
{
   class connection;
   class table_reader;
   class table_writer;
   class transaction_base;

   class result final
   {
   private:
      friend class connection;
      friend class table_reader;
      friend class table_writer;
      friend class transaction_base;

      const std::shared_ptr< PGresult > m_pgresult;
      const std::size_t m_columns;
      const std::size_t m_rows;

      void check_row( const std::size_t row ) const;

      explicit result( PGresult* pgresult );

   public:
      [[nodiscard]] auto status() const noexcept -> result_status;
      [[nodiscard]] auto has_rows_affected() const noexcept -> bool;
      [[nodiscard]] auto rows_affected() const -> std::size_t;

      [[nodiscard]] auto columns() const noexcept -> std::size_t
      {
         return m_columns;
      }

      [[nodiscard]] auto name( const std::size_t column ) const -> std::string;
      [[nodiscard]] auto index( const internal::zsv in_name ) const -> std::size_t;

      [[nodiscard]] auto size() const noexcept -> std::size_t
      {
         assert( m_columns != 0 );
         return m_rows;
      }

      [[nodiscard]] auto empty() const noexcept -> bool
      {
         return size() == 0;
      }

   private:
      class const_iterator
         : private row
      {
      private:
         friend class result;

         explicit const_iterator( const row& r ) noexcept
            : row( r )
         {}

      public:
         using difference_type = std::int32_t;
         using value_type = const row;
         using pointer = const row*;
         using reference = const row&;
         using iterator_category = std::random_access_iterator_tag;

         const_iterator() = default;

         auto operator++() noexcept -> const_iterator&
         {
            ++m_row;
            return *this;
         }

         auto operator++( int ) noexcept -> const_iterator
         {
            const_iterator nrv( *this );
            ++*this;
            return nrv;
         }

         auto operator+=( const difference_type n ) noexcept -> const_iterator&
         {
            m_row += n;
            return *this;
         }

         auto operator--() noexcept -> const_iterator&
         {
            --m_row;
            return *this;
         }

         auto operator--( int ) noexcept -> const_iterator
         {
            const_iterator nrv( *this );
            --*this;
            return nrv;
         }

         auto operator-=( const difference_type n ) noexcept -> const_iterator&
         {
            m_row -= n;
            return *this;
         }

         [[nodiscard]] auto operator*() const noexcept -> const row&
         {
            return *this;
         }

         [[nodiscard]] auto operator->() const noexcept -> const row*
         {
            return this;
         }

         [[nodiscard]] auto operator[]( const difference_type n ) const noexcept -> row
         {
            return *( *this + n );
         }

         friend void swap( const_iterator& lhs, const_iterator& rhs ) noexcept
         {
            swap( static_cast< row& >( lhs ), static_cast< row& >( rhs ) );
         }

         [[nodiscard]] friend auto operator+( const const_iterator& lhs, const difference_type rhs ) noexcept -> const_iterator
         {
            const_iterator nrv( lhs );
            nrv += rhs;
            return nrv;
         }

         [[nodiscard]] friend auto operator+( const difference_type lhs, const const_iterator& rhs ) noexcept -> const_iterator
         {
            const_iterator nrv( rhs );
            nrv += lhs;
            return nrv;
         }

         [[nodiscard]] friend auto operator-( const const_iterator& lhs, const difference_type rhs ) noexcept -> const_iterator
         {
            const_iterator nrv( lhs );
            nrv -= rhs;
            return nrv;
         }

         [[nodiscard]] friend auto operator-( const const_iterator& lhs, const const_iterator& rhs ) noexcept -> difference_type
         {
            return static_cast< difference_type >( lhs.m_row ) - static_cast< difference_type >( rhs.m_row );
         }

         [[nodiscard]] friend auto operator==( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_row == rhs.m_row;
         }

         [[nodiscard]] friend auto operator<=>( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_row <=> rhs.m_row;
         }
      };

   public:
      [[nodiscard]] auto begin() const noexcept -> const_iterator;
      [[nodiscard]] auto end() const noexcept -> const_iterator;

      [[nodiscard]] auto cbegin() const noexcept
      {
         return begin();
      }

      [[nodiscard]] auto cend() const noexcept
      {
         return end();
      }

      [[nodiscard]] auto is_null( const std::size_t row, const std::size_t column ) const -> bool;
      [[nodiscard]] auto get( const std::size_t row, const std::size_t column ) const -> const char*;

      [[nodiscard]] auto operator[]( const std::size_t row ) const noexcept
      {
         return pq::row( *this, row, 0, m_columns );
      }

      [[nodiscard]] auto at( const std::size_t row ) const -> pq::row;

      template< result_type T >
      [[nodiscard]] auto as() const -> T
      {
         switch( size() ) {
            case 0:
               throw std::runtime_error( "invalid empty result, expected 1 row" );

            case 1:
               return ( *this )[ 0 ].as< T >();

            default:
               throw std::runtime_error( std::format( "invalid result size: {} rows, expected 1 row", m_rows ) );
         }
      }

      template< result_type T >
      [[nodiscard]] auto optional() const -> std::optional< T >
      {
         switch( size() ) {
            case 0:
               return std::nullopt;

            case 1:
               return ( *this )[ 0 ].as< T >();

            default:
               throw std::runtime_error( std::format( "invalid result size: {} rows, expected 0 or 1 rows", m_rows ) );
         }
      }

      template< result_type T, result_type U >
      [[nodiscard]] auto pair() const
      {
         return as< std::pair< T, U > >();
      }

      template< result_type... Ts >
      [[nodiscard]] auto tuple() const
      {
         return as< std::tuple< Ts... > >();
      }

      template< typename T >
         requires result_type< typename T::value_type >
      [[nodiscard]] auto as_container() const -> T
      {
         assert( m_columns != 0 );
         T nrv;
         if constexpr( requires { nrv.reserve( size() ); } ) {
            nrv.reserve( size() );
         }
         for( const auto& row : *this ) {
            nrv.insert( nrv.end(), row.as< typename T::value_type >() );
         }
         return nrv;
      }

      template< typename... Ts >
         requires result_type< typename std::vector< Ts... >::value_type >
      [[nodiscard]] auto vector() const
      {
         return as_container< std::vector< Ts... > >();
      }

      template< typename... Ts >
         requires result_type< typename std::list< Ts... >::value_type >
      [[nodiscard]] auto list() const
      {
         return as_container< std::list< Ts... > >();
      }

      template< typename... Ts >
         requires result_type< typename std::set< Ts... >::value_type >
      [[nodiscard]] auto set() const
      {
         return as_container< std::set< Ts... > >();
      }

      template< typename... Ts >
         requires result_type< typename std::multiset< Ts... >::value_type >
      [[nodiscard]] auto multiset() const
      {
         return as_container< std::multiset< Ts... > >();
      }

      template< typename... Ts >
         requires result_type< typename std::unordered_set< Ts... >::value_type >
      [[nodiscard]] auto unordered_set() const
      {
         return as_container< std::unordered_set< Ts... > >();
      }

      template< typename... Ts >
         requires result_type< typename std::unordered_multiset< Ts... >::value_type >
      [[nodiscard]] auto unordered_multiset() const
      {
         return as_container< std::unordered_multiset< Ts... > >();
      }

      template< typename... Ts >
         requires result_type< typename std::map< Ts... >::value_type >
      [[nodiscard]] auto map() const
      {
         return as_container< std::map< Ts... > >();
      }

      template< typename... Ts >
         requires result_type< typename std::multimap< Ts... >::value_type >
      [[nodiscard]] auto multimap() const
      {
         return as_container< std::multimap< Ts... > >();
      }

      template< typename... Ts >
         requires result_type< typename std::unordered_map< Ts... >::value_type >
      [[nodiscard]] auto unordered_map() const
      {
         return as_container< std::unordered_map< Ts... > >();
      }

      template< typename... Ts >
         requires result_type< typename std::unordered_multimap< Ts... >::value_type >
      [[nodiscard]] auto unordered_multimap() const
      {
         return as_container< std::unordered_multimap< Ts... > >();
      }

      [[nodiscard]] auto underlying_raw_ptr() noexcept -> PGresult*
      {
         return m_pgresult.get();
      }

      [[nodiscard]] auto underlying_raw_ptr() const noexcept -> const PGresult*
      {
         return m_pgresult.get();
      }
   };

}  // namespace tao::pq

#endif
