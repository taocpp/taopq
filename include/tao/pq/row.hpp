// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_ROW_HPP
#define TAO_PQ_ROW_HPP

#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

#include <tao/pq/field.hpp>
#include <tao/pq/internal/demangle.hpp>
#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/is_aggregate.hpp>
#include <tao/pq/result_traits.hpp>

namespace tao::pq
{
   class result;

   class row
   {
   protected:
      friend class field;
      friend class result;

      const result* m_result = nullptr;
      std::size_t m_row = 0;
      std::size_t m_offset = 0;
      std::size_t m_columns = 0;

      row() = default;

      row( const result& in_result, const std::size_t in_row, const std::size_t in_offset, const std::size_t in_columns ) noexcept
         : m_result( &in_result ),
           m_row( in_row ),
           m_offset( in_offset ),
           m_columns( in_columns )
      {}

      void ensure_column( const std::size_t column ) const;

   public:
      [[nodiscard]] auto slice( const std::size_t offset, const std::size_t in_columns ) const -> row;

      [[nodiscard]] auto columns() const noexcept -> std::size_t
      {
         return m_columns;
      }

      [[nodiscard]] auto name( const std::size_t column ) const -> std::string;
      [[nodiscard]] auto index( const internal::zsv in_name ) const -> std::size_t;

   private:
      class const_iterator
         : private field
      {
      private:
         friend class row;

         explicit const_iterator( const field& f ) noexcept
            : field( f )
         {}

      public:
         using difference_type = std::int32_t;
         using value_type = const field;
         using pointer = const field*;
         using reference = const field&;
         using iterator_category = std::random_access_iterator_tag;

         const_iterator() = default;

         auto operator++() noexcept -> const_iterator&
         {
            ++m_column;
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
            m_column += n;
            return *this;
         }

         auto operator--() noexcept -> const_iterator&
         {
            --m_column;
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
            m_column -= n;
            return *this;
         }

         [[nodiscard]] auto operator*() const noexcept -> const field&
         {
            return *this;
         }

         [[nodiscard]] auto operator->() const noexcept -> const field*
         {
            return this;
         }

         [[nodiscard]] auto operator[]( const difference_type n ) const noexcept -> field
         {
            return *( *this + n );
         }

         friend void swap( const_iterator& lhs, const_iterator& rhs ) noexcept
         {
            swap( static_cast< field& >( lhs ), static_cast< field& >( rhs ) );
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
            return static_cast< difference_type >( lhs.index() ) - static_cast< difference_type >( rhs.index() );
         }

         [[nodiscard]] friend auto operator==( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.index() == rhs.index();
         }

         [[nodiscard]] friend auto operator<=>( const const_iterator& lhs, const const_iterator& rhs ) noexcept
         {
            return lhs.index() <=> rhs.index();
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

      [[nodiscard]] auto is_null( const std::size_t column ) const -> bool;
      [[nodiscard]] auto get( const std::size_t column ) const -> const char*;

      template< result_type_direct T >
      [[nodiscard]] auto get( const std::size_t column ) const -> T
      {
         if constexpr( requires { result_traits< T >::null(); } ) {
            if( is_null( column ) ) {
               return result_traits< T >::null();
            }
         }
         return result_traits< T >::from( get( column ) );
      }

      template< result_type_composite T >
      [[nodiscard]] auto get( const std::size_t column ) const -> T
      {
         return result_traits< T >::from( slice( column, result_traits_size< T > ) );
      }

      template< result_type T >
      [[nodiscard]] auto optional( const std::size_t column ) const
      {
         return get< std::optional< T > >( column );
      }

      template< result_type T >
      [[nodiscard]] auto as() const -> T
      {
         if( result_traits_size< T > != m_columns ) {
            const auto type = internal::demangle< T >();
            throw std::out_of_range( std::format( "datatype '{}' requires {} columns, but row/slice has {} columns", type, result_traits_size< T >, m_columns ) );
         }
         return get< T >( 0 );
      }

      template< typename T >
         requires is_aggregate_result< T >
      [[nodiscard]] operator T() const
      {
         return as< T >();
      }

      template< result_type T >
      [[nodiscard]] auto optional() const
      {
         return as< std::optional< T > >();
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

      [[nodiscard]] auto at( const std::size_t column ) const -> field;

      [[nodiscard]] auto operator[]( const std::size_t column ) const noexcept -> field
      {
         return { *this, m_offset + column };
      }

      [[nodiscard]] auto at( const internal::zsv in_name ) const -> field
      {
         // row::index does the necessary checks, so we forward to operator[]
         return ( *this )[ row::index( in_name ) ];
      }

      [[nodiscard]] auto operator[]( const internal::zsv in_name ) const -> field
      {
         return ( *this )[ row::index( in_name ) ];
      }

      friend void swap( row& lhs, row& rhs ) noexcept
      {
         std::swap( lhs.m_result, rhs.m_result );
         std::swap( lhs.m_row, rhs.m_row );
         std::swap( lhs.m_offset, rhs.m_offset );
         std::swap( lhs.m_columns, rhs.m_columns );
      }
   };

   template< result_type T >
   auto field::as() const -> T
   {
      static_assert( result_traits_size< T > == 1, "tao::pq::result_traits<T>::size does not yield exactly one column for T, which is required for field access" );
      return m_row->get< T >( m_column );
   }

}  // namespace tao::pq

#endif
