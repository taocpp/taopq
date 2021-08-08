// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TABLE_ROW_HPP
#define TAO_PQ_TABLE_ROW_HPP

#include <optional>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/demangle.hpp>
#include <tao/pq/internal/dependent_false.hpp>
#include <tao/pq/internal/printf.hpp>
#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/result_traits.hpp>
#include <tao/pq/table_field.hpp>

namespace tao::pq
{
   class table_reader;

   class table_row
   {
   protected:
      friend class table_reader;

      table_reader& m_reader;
      std::size_t m_offset;
      std::size_t m_columns;

      table_row( table_reader& in_reader, const std::size_t in_offset, const std::size_t in_columns ) noexcept
         : m_reader( in_reader ),
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

      [[nodiscard]] auto is_null( const std::size_t column ) const -> bool;
      [[nodiscard]] auto get( const std::size_t column ) const -> std::string_view;

      template< typename T >
      [[nodiscard]] auto get( const std::size_t /*unused*/ ) const noexcept
         -> std::enable_if_t< result_traits_size< T > == 0, T >
      {
         static_assert( internal::dependent_false< T >, "tao::pq::result_traits<T>::size yields zero" );
         TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
      }

      template< typename T >
      [[nodiscard]] auto get( const std::size_t column ) const
         -> std::enable_if_t< result_traits_size< T > == 1 && result_traits_has_null< T >, T >
      {
         if( is_null( column ) ) {
            return result_traits< T >::null();
         }
         return internal::result_traits_from< T >( get( column ) );
      }

      template< typename T >
      [[nodiscard]] auto get( const std::size_t column ) const
         -> std::enable_if_t< result_traits_size< T > == 1 && !result_traits_has_null< T >, T >
      {
         ensure_column( column );
         return internal::result_traits_from< T >( get( column ) );
      }

      template< typename T >
      [[nodiscard]] auto get( const std::size_t column ) const
         -> std::enable_if_t< ( ( result_traits_size< T > ) > 1 ), T >
      {
         return result_traits< T >::from( slice( column, result_traits_size< T > ) );
      }

      template< typename T >
      [[nodiscard]] auto optional( const std::size_t column ) const
      {
         return get< std::optional< T > >( column );
      }

      template< typename T >
      [[nodiscard]] auto as() const -> T
      {
         if( result_traits_size< T > != m_columns ) {
            throw std::runtime_error( internal::printf( "datatype (%s) requires %zu columns, but table_row/slice has %zu columns", internal::demangle< T >().c_str(), result_traits_size< T >, m_columns ) );
         }
         return get< T >( 0 );
      }

      template< typename T >
      [[nodiscard]] auto optional() const
      {
         return as< std::optional< T > >();
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

      [[nodiscard]] auto at( const std::size_t column ) const -> table_field;

      [[nodiscard]] auto operator[]( const std::size_t column ) const noexcept -> table_field
      {
         return table_field( *this, m_offset + column );
      }
   };

   template< typename T >
   auto table_field::as() const
      -> std::enable_if_t< result_traits_size< T > == 1, T >
   {
      return m_row.get< T >( m_column );
   }

}  // namespace tao::pq

#endif
