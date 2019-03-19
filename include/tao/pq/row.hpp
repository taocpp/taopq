// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_ROW_HPP
#define TAO_PQ_ROW_HPP

#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/field.hpp>
#include <tao/pq/internal/demangle.hpp>
#include <tao/pq/internal/printf.hpp>
#include <tao/pq/result_traits.hpp>

namespace tao::pq
{
   class result;

   class row
   {
   protected:
      friend class result;

      const result& result_;
      std::size_t row_;
      const std::size_t offset_;
      const std::size_t columns_;

      row( const result& in_result, const std::size_t in_row, const std::size_t in_offset, const std::size_t in_columns )
         : result_( in_result ),
           row_( in_row ),
           offset_( in_offset ),
           columns_( in_columns )
      {
      }

      void ensure_column( const std::size_t column ) const;

   public:
      [[nodiscard]] row slice( const std::size_t offset, const std::size_t in_columns ) const;

      [[nodiscard]] std::size_t columns() const
      {
         return columns_;
      }

      [[nodiscard]] std::string name( const std::size_t column ) const;
      [[nodiscard]] std::size_t index( const std::string& in_name ) const;

      [[nodiscard]] bool is_null( const std::size_t column ) const;
      [[nodiscard]] const char* get( const std::size_t column ) const;

      template< typename T >
      [[nodiscard]] std::enable_if_t< result_traits_size< T > == 0, T > get( const std::size_t ) const
      {
         static_assert( !std::is_same< T, T >::value, "tao::pq::result_traits<T>::size yields zero" );
         __builtin_unreachable();
      }

      template< typename T >
      [[nodiscard]] std::enable_if_t< result_traits_size< T > == 1 && result_traits_has_null< T >, T > get( const std::size_t column ) const
      {
         if( is_null( column ) ) {
            return result_traits< T >::null();
         }
         return result_traits< T >::from( get( column ) );
      }

      template< typename T >
      [[nodiscard]] std::enable_if_t< result_traits_size< T > == 1 && !result_traits_has_null< T >, T > get( const std::size_t column ) const
      {
         ensure_column( column );
         return result_traits< T >::from( get( column ) );
      }

      template< typename T >
      [[nodiscard]] std::enable_if_t< ( ( result_traits_size< T > ) > 1 ), T > get( const std::size_t column ) const
      {
         return result_traits< T >::from( slice( column, result_traits_size< T > ) );
      }

      template< typename T >
      [[nodiscard]] std::optional< T > optional( const std::size_t column ) const
      {
         return get< std::optional< T > >( column );
      }

      template< typename T >
      [[nodiscard]] T as() const
      {
         if( result_traits_size< T > != columns_ ) {
            throw std::runtime_error( internal::printf( "datatype (%s) requires %zu columns, but row/slice has %zu columns", internal::demangle< T >().c_str(), result_traits_size< T >, columns_ ) );
         }
         return get< T >( 0 );
      }

      template< typename T >
      [[nodiscard]] std::optional< T > optional() const
      {
         return as< std::optional< T > >();
      }

      template< typename T, typename U >
      [[nodiscard]] std::pair< T, U > pair() const
      {
         return as< std::pair< T, U > >();
      }

      template< typename... Ts >
      [[nodiscard]] std::tuple< Ts... > tuple() const
      {
         return as< std::tuple< Ts... > >();
      }

      [[nodiscard]] field operator[]( const std::size_t column ) const
      {
         ensure_column( column );
         return field( *this, offset_ + column );
      }

      [[nodiscard]] field operator[]( const std::string& in_name ) const
      {
         return ( *this )[ index( in_name ) ];
      }

      [[nodiscard]] field at( const std::size_t column ) const;
      [[nodiscard]] field at( const std::string& in_name ) const;
   };

   template< typename T >
   std::enable_if_t< result_traits_size< T > == 1, T > field::as() const
   {
      return row_.get< T >( column_ );
   }

}  // namespace tao::pq

#endif
