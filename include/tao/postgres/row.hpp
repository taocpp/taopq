// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_POSTGRES_ROW_HPP
#define TAOCPP_INCLUDE_POSTGRES_ROW_HPP

#include <string>
#include <stdexcept>
#include <utility>
#include <tuple>
#include <type_traits>
#include <tao/postgres/result_traits.hpp>
#include <tao/postgres/field.hpp>
#include <tao/utility/printf.hpp>
#include <tao/utility/demangle.hpp>

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

      row( const result& result, const std::size_t row, const std::size_t offset, const std::size_t columns )
        : result_( result ),
          row_( row ),
          offset_( offset ),
          columns_( columns )
      {
      }

      void ensure_column( const std::size_t column ) const;

    public:
      row slice( const std::size_t offset, const std::size_t columns ) const;

      std::size_t columns() const
      {
        return columns_;
      }

      std::string name( const std::size_t column ) const;
      std::size_t index( const std::string& name ) const;

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

      template< typename T, typename U >
      std::pair< T, U > pair( const std::size_t column ) const
      {
        return get< std::pair< T, U > >( column );
      }

      template< typename... Ts >
      std::tuple< Ts... > tuple( const std::size_t column ) const
      {
        return get< std::tuple< Ts... > >( column );
      }

      template< typename T >
      T as() const
      {
        if( result_traits_size< T >::value != columns_ ) {
          throw std::runtime_error( utility::printf( "datatype (%s) requires %lu columns, but row/slice has %lu columns", utility::demangle< T >().c_str(), result_traits_size< T >::value, columns_ ) );
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

      field operator[]( const std::string& name ) const
      {
        return (*this)[ index( name ) ];
      }

      field at( const std::size_t column ) const;
      field at( const std::string& name ) const;
    };

    template< typename T >
    typename std::enable_if< result_traits_size< T >::value == 1, T >::type field::as() const
    {
      return row_.get< T >( column_ );
    }
  }
}

#endif // TAOCPP_INCLUDE_POSTGRES_ROW_HPP
