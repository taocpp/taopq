// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/result.hpp>
#include <tao/pq/row.hpp>

namespace tao::pq
{
   void row::ensure_column( const std::size_t column ) const
   {
      if( column >= m_columns ) {
         throw std::out_of_range( internal::printf( "column %zu out of range (0-%zu)", column, m_columns - 1 ) );
      }
   }

   row row::slice( const std::size_t offset, const std::size_t in_columns ) const
   {
      if( in_columns == 0 ) {
         throw std::invalid_argument( "slice requires at least one column" );
      }
      if( offset + in_columns > m_columns ) {
         throw std::out_of_range( internal::printf( "slice (%zu-%zu) out of range (0-%zu)", offset, offset + in_columns - 1, m_columns - 1 ) );
      }
      return row( m_result, m_row, m_offset + offset, in_columns );
   }

   std::string row::name( const std::size_t column ) const
   {
      return m_result.name( m_offset + column );
   }

   std::size_t row::index( const std::string& in_name ) const
   {
      const std::size_t n = m_result.index( in_name );
      if( n >= m_offset ) {
         if( n - m_offset < m_columns ) {
            return n - m_offset;
         }
      }
      else {
         const std::string adapted_name = m_result.name( n );
         for( std::size_t pos = 0; pos < m_columns; ++pos ) {
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
      return m_result.is_null( m_row, m_offset + column );
   }

   const char* row::get( const std::size_t column ) const
   {
      ensure_column( column );
      return m_result.get( m_row, m_offset + column );
   }

}  // namespace tao::pq
