// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_ROW_HPP
#define TAO_POSTGRES_ROW_HPP

#include <stdexcept>
#include <string>
#include <tao/postgres/field.hpp>
#include <tao/postgres/result_traits.hpp>
#include <tao/utility/demangle.hpp>
#include <tao/utility/printf.hpp>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tao
{
   namespace postgres
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
         row slice( const std::size_t offset, const std::size_t in_columns ) const;

         std::size_t columns() const
         {
            return columns_;
         }

         std::string name( const std::size_t column ) const;
         std::size_t index( const std::string& in_name ) const;

         bool is_null( const std::size_t column ) const;
         const char* get( const std::size_t column ) const;

         template< typename T >
         typename std::enable_if< result_traits_size< T >::value == 0, T >::type get( const std::size_t ) const
         {
            static_assert( !std::is_same< T, T >::value, "tao::postgres::result_traits<T>::size yields zero" );
            __builtin_unreachable();
         }

         template< typename T >
         typename std::enable_if< result_traits_size< T >::value == 1 && result_traits_has_null< T >::value, T >::type get( const std::size_t column ) const
         {
            if( is_null( column ) ) {
               return result_traits< T >::null();
            }
            return result_traits< T >::from( get( column ) );
         }

         template< typename T >
         typename std::enable_if< result_traits_size< T >::value == 1 && !result_traits_has_null< T >::value, T >::type get( const std::size_t column ) const
         {
            ensure_column( column );
            return result_traits< T >::from( get( column ) );
         }

         template< typename T >
         typename std::enable_if< ( result_traits_size< T >::value > 1 ), T >::type get( const std::size_t column ) const
         {
            return result_traits< T >::from( slice( column, result_traits_size< T >::value ) );
         }

         template< typename T >
         tao::optional< T > optional( const std::size_t column ) const
         {
            return get< tao::optional< T > >( column );
         }

         template< typename T >
         T as() const
         {
            if( result_traits_size< T >::value != columns_ ) {
               throw std::runtime_error( utility::printf( "datatype (%s) requires %zu columns, but row/slice has %zu columns", utility::demangle< T >().c_str(), result_traits_size< T >::value, columns_ ) );
            }
            return get< T >( 0 );
         }

         template< typename T >
         tao::optional< T > optional() const
         {
            return as< tao::optional< T > >();
         }

         template< typename T, typename U >
         std::pair< T, U > pair() const
         {
            return as< std::pair< T, U > >();
         }

         template< typename... Ts >
         std::tuple< Ts... > tuple() const
         {
            return as< std::tuple< Ts... > >();
         }

         field operator[]( const std::size_t column ) const
         {
            ensure_column( column );
            return field( *this, offset_ + column );
         }

         field operator[]( const std::string& in_name ) const
         {
            return ( *this )[ index( in_name ) ];
         }

         field at( const std::size_t column ) const;
         field at( const std::string& in_name ) const;
      };

      template< typename T >
      typename std::enable_if< result_traits_size< T >::value == 1, T >::type field::as() const
      {
         return row_.get< T >( column_ );
      }

   }  // namespace postgres

}  // namespace tao

#endif
