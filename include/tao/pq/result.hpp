// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_RESULT_HPP
#define TAO_PQ_RESULT_HPP

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
#include <tao/pq/row.hpp>

namespace tao::pq
{
   class table_writer;

   namespace internal
   {
      class connection;

      template< typename T, typename = void >
      inline constexpr bool has_reserve = false;

      template< typename T >
      inline constexpr bool has_reserve< T, std::void_t< decltype( T::reserve( std::declval< typename T::size_type >() ) ) > > = true;

   }  // namespace internal

   class result
   {
   private:
      friend class internal::connection;
      friend class table_writer;

      const std::shared_ptr< PGresult > m_pgresult;
      const std::size_t m_columns;
      const std::size_t m_rows;

      void check_has_result_set() const;
      void check_row( const std::size_t row ) const;

      enum class mode_t
      {
         expect_ok,
         expect_copy_in
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
      [[nodiscard]] auto index( const std::string& in_name ) const -> std::size_t;

      [[nodiscard]] auto empty() const -> bool;
      [[nodiscard]] auto size() const -> std::size_t;

      class const_iterator
         : private row
      {
      private:
         friend class result;

         const_iterator( const row& r ) noexcept
            : row( r )
         {}

      public:
         [[nodiscard]] friend auto operator!=( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.m_row != rhs.m_row;
         }

         auto operator++() noexcept -> const_iterator&
         {
            ++m_row;
            return *this;
         }

         [[nodiscard]] auto operator*() const noexcept -> const row&
         {
            return *this;
         }
      };

      [[nodiscard]] auto begin() const -> const_iterator;
      [[nodiscard]] auto end() const -> const_iterator;

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
            return {};
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

      [[nodiscard]] auto underlying_raw_ptr() -> PGresult*
      {
         return m_pgresult.get();
      }

      [[nodiscard]] auto underlying_raw_ptr() const -> const PGresult*
      {
         return m_pgresult.get();
      }
   };

}  // namespace tao::pq

#endif
