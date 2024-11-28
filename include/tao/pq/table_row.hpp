// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_TABLE_ROW_HPP
#define TAO_PQ_TABLE_ROW_HPP

#include <cstddef>
#include <cstdint>
#include <format>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <utility>

#include <tao/pq/internal/demangle.hpp>
#include <tao/pq/result_traits.hpp>
#include <tao/pq/table_field.hpp>

namespace tao::pq
{
   class table_reader;

   class table_row
   {
   protected:
      friend class table_field;
      friend class table_reader;

      table_reader* m_reader;
      std::size_t m_offset;
      std::size_t m_columns;

      table_row( table_reader& in_reader, const std::size_t in_offset, const std::size_t in_columns ) noexcept
         : m_reader( &in_reader ),
           m_offset( in_offset ),
           m_columns( in_columns )
      {}

      void ensure_column( const std::size_t column ) const;

   public:
      [[nodiscard]] auto slice( const std::size_t offset, const std::size_t in_columns ) const -> table_row;

      [[nodiscard]] auto columns() const noexcept -> std::size_t
      {
         return m_columns;
      }

   private:
      class const_iterator
         : private table_field
      {
      private:
         friend class table_row;

         explicit const_iterator( const table_field& f ) noexcept
            : table_field( f )
         {}

      public:
         using difference_type = std::int32_t;
         using value_type = const table_field;
         using pointer = const table_field*;
         using reference = const table_field&;
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

         [[nodiscard]] auto operator*() const noexcept -> const table_field&
         {
            return *this;
         }

         [[nodiscard]] auto operator->() const noexcept -> const table_field*
         {
            return this;
         }

         [[nodiscard]] auto operator[]( const difference_type n ) const noexcept -> table_field
         {
            return *( *this + n );
         }

         friend void swap( const_iterator& lhs, const_iterator& rhs ) noexcept
         {
            swap( static_cast< table_field& >( lhs ), static_cast< table_field& >( rhs ) );
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
         const char* const value = get( column );
         if( value == nullptr ) {
            if constexpr( requires { result_traits< T >::null(); } ) {
               return result_traits< T >::null();
            }
            else {
               throw std::invalid_argument( "unexpected NULL value" );
            }
         }
         return result_traits< T >::from( value );
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
            throw std::out_of_range( std::format( "datatype '{}' requires {} columns, but table_row/slice has {} columns", internal::demangle< T >(), result_traits_size< T >, m_columns ) );
         }
         return get< T >( 0 );
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

      [[nodiscard]] auto at( const std::size_t column ) const -> table_field;

      [[nodiscard]] auto operator[]( const std::size_t column ) const noexcept -> table_field
      {
         return { *this, m_offset + column };
      }

      friend void swap( table_row& lhs, table_row& rhs ) noexcept
      {
         std::swap( lhs.m_reader, rhs.m_reader );
         std::swap( lhs.m_offset, rhs.m_offset );
         std::swap( lhs.m_columns, rhs.m_columns );
      }
   };

   template< result_type T >
   auto table_field::as() const -> T
   {
      static_assert( result_traits_size< T > == 1, "tao::pq::result_traits<T>::size does not yield exactly one column for T, which is required for field access" );
      return m_row->get< T >( m_column );
   }

}  // namespace tao::pq

#endif
