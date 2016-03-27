// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#include <cassert>

#include <tao/postgres/row.hpp>
#include <tao/postgres/result.hpp>

namespace tao
{
  namespace postgres
  {
    row row::slice( const std::size_t offset, const std::size_t columns ) const
    {
      assert( columns > 0 );
      assert( offset + columns <= columns_ );
      return row( result_, row_, offset_ + offset, columns );
    }

    std::string row::name( const std::size_t column ) const
    {
      return result_.name( offset_ + column );
    }

    std::size_t row::index( const std::string& name ) const
    {
      const std::size_t n = result_.index( name );
      if( n >= offset_ ) {
        if ( n - offset_ < columns_ ) {
          return n - offset_;
        }
      }
      else {
        const std::string adapted_name = result_.name( n );
        for( std::size_t pos = 0; pos < columns_; ++pos ) {
          if( this->name( pos ) == adapted_name ) {
            return pos;
          }
        }
      }
      throw std::runtime_error( "column not found: " + name );
    }

    bool row::is_null( const std::size_t column ) const
    {
      assert( column < columns_ );
      return result_.is_null( row_, offset_ + column );
    }

    const char* row::get( const std::size_t column ) const
    {
      assert( column < columns_ );
      return result_.get( row_, offset_ + column );
    }
  }
}
