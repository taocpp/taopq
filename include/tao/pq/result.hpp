// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_HPP
#define TAO_PQ_RESULT_HPP

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

#include <tao/pq/internal/printf.hpp>
#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/row.hpp>

namespace tao::pq
{
   class connection;
   class table_reader;
   class table_writer;
   class transaction;

   namespace internal
   {
      template< typename T, typename = void >
      inline constexpr bool has_reserve = false;

      template< typename T >
      inline constexpr bool has_reserve< T, decltype( std::declval< T >().reserve( std::declval< typename T::size_type >() ), void() ) > = true;

   }  // namespace internal

   class result final
   {
   private:
      friend class connection;
      friend class table_reader;
      friend class table_writer;
      friend class transaction;

      const std::shared_ptr< PGresult > m_pgresult;
      const std::size_t m_columns;
      const std::size_t m_rows;

      void check_has_result_set() const;
      void check_row( const std::size_t row ) const;

      enum class mode_t
      {
         expect_ok,
         expect_copy_in,
         expect_copy_out
      };

      result( PGresult* pgresult, const mode_t mode = mode_t::expect_ok );

   public:
      [[nodiscard]] auto has_rows_affected() const noexcept -> bool;
      [[nodiscard]] auto rows_affected() const -> std::size_t;

      [[nodiscard]] auto columns() const noexcept -> std::size_t
      {
         return m_columns;
      }

      [[nodiscard]] auto name( const std::size_t column ) const -> std::string;
      [[nodiscard]] auto index( const internal::zsv in_name ) const -> std::size_t;

      [[nodiscard]] auto empty() const -> bool;
      [[nodiscard]] auto size() const -> std::size_t;

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
            return swap( static_cast< row& >( lhs ), static_cast< row& >( rhs ) );
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

         [[nodiscard]] friend auto operator!=( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_row != rhs.m_row;
         }

         [[nodiscard]] friend auto operator<( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_row < rhs.m_row;
         }

         [[nodiscard]] friend auto operator>( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_row > rhs.m_row;
         }

         [[nodiscard]] friend auto operator<=( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_row <= rhs.m_row;
         }

         [[nodiscard]] friend auto operator>=( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_row >= rhs.m_row;
         }
      };

   public:
      [[nodiscard]] auto begin() const -> const_iterator;
      [[nodiscard]] auto end() const -> const_iterator;

      [[nodiscard]] auto cbegin() const
      {
         return begin();
      }

      [[nodiscard]] auto cend() const
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

      template< typename T >
      [[nodiscard]] auto as() const -> T
      {
         if( size() != 1 ) {
            throw std::runtime_error( internal::printf( "invalid result size: %zu rows, expected 1 row", m_rows ) );
         }
         return ( *this )[ 0 ].as< T >();
      }

      template< typename T >
      [[nodiscard]] auto optional() const -> std::optional< T >
      {
         if( empty() ) {
            return std::nullopt;
         }
         return as< T >();
      }

      template< typename T, typename U >
      [[nodiscard]] auto pair() const
      {
         return as< std::pair< T, U > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto tuple() const
      {
         return as< std::tuple< Ts... > >();
      }

      template< typename T >
      [[nodiscard]] auto as_container() const -> T
      {
         T nrv;
         if constexpr( internal::has_reserve< T > ) {
            nrv.reserve( size() );
         }
         check_has_result_set();
         for( const auto& row : *this ) {
            nrv.insert( nrv.end(), row.as< typename T::value_type >() );
         }
         return nrv;
      }

      template< typename... Ts >
      [[nodiscard]] auto vector() const
      {
         return as_container< std::vector< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto list() const
      {
         return as_container< std::list< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto set() const
      {
         return as_container< std::set< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto multiset() const
      {
         return as_container< std::multiset< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto unordered_set() const
      {
         return as_container< std::unordered_set< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto unordered_multiset() const
      {
         return as_container< std::unordered_multiset< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto map() const
      {
         return as_container< std::map< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto multimap() const
      {
         return as_container< std::multimap< Ts... > >();
      }

      template< typename... Ts >
      [[nodiscard]] auto unordered_map() const
      {
         return as_container< std::unordered_map< Ts... > >();
      }

      template< typename... Ts >
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
