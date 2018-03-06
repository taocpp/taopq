// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#include <tao/postgres/result.hpp>
#include <tao/postgres/row.hpp>

namespace tao
{
   namespace postgres
   {
      void row::ensure_column( const std::size_t column ) const
      {
         if( column >= columns_ ) {
            throw std::out_of_range( utility::printf( "column %zu out of range (0-%zu)", column, columns_ - 1 ) );
         }
      }

      row row::slice( const std::size_t offset, const std::size_t in_columns ) const
      {
         if( in_columns == 0 ) {
            throw std::invalid_argument( "slice requires at least one column" );
         }
         if( offset + in_columns > columns_ ) {
            throw std::out_of_range( utility::printf( "slice (%zu-%zu) out of range (0-%zu)", offset, offset + in_columns - 1, columns_ - 1 ) );
         }
         return row( result_, row_, offset_ + offset, in_columns );
      }

      std::string row::name( const std::size_t column ) const
      {
         return result_.name( offset_ + column );
      }

      std::size_t row::index( const std::string& in_name ) const
      {
         const std::size_t n = result_.index( in_name );
         if( n >= offset_ ) {
            if( n - offset_ < columns_ ) {
               return n - offset_;
            }
         }
         else {
            const std::string adapted_name = result_.name( n );
            for( std::size_t pos = 0; pos < columns_; ++pos ) {
               if( name( pos ) == adapted_name ) {
                  return pos;
               }
            }
         }
         throw std::out_of_range( "column not found: " + in_name );
      }

      bool row::is_null( const std::size_t column ) const
      {
         ensure_column( column );
         return result_.is_null( row_, offset_ + column );
      }

      const char* row::get( const std::size_t column ) const
      {
         ensure_column( column );
         return result_.get( row_, offset_ + column );
      }

   }  // namespace postgres

}  // namespace tao
