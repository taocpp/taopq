// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/table_reader.hpp>
#include <tao/pq/table_row.hpp>

namespace tao::pq
{
   void table_row::ensure_column( const std::size_t column ) const
   {
      if( column >= m_columns ) {
         throw std::out_of_range( internal::printf( "column %zu out of range (0-%zu)", column, m_columns - 1 ) );
      }
   }

   auto table_row::slice( const std::size_t offset, const std::size_t in_columns ) const -> table_row
   {
      if( in_columns == 0 ) {
         throw std::invalid_argument( "slice requires at least one column" );
      }
      if( offset + in_columns > m_columns ) {
         throw std::out_of_range( internal::printf( "slice (%zu-%zu) out of range (0-%zu)", offset, offset + in_columns - 1, m_columns - 1 ) );
      }
      return table_row( m_reader, m_offset + offset, in_columns );
   }

   auto table_row::is_null( const std::size_t column ) const -> bool
   {
      return get( column ) == nullptr;
   }

   auto table_row::get( const std::size_t column ) const -> const char*
   {
      ensure_column( column );
      return m_reader.raw_data()[ m_offset + column ];
   }

   auto table_row::at( const std::size_t column ) const -> table_field
   {
      ensure_column( column );
      return table_field( *this, m_offset + column );
   }

}  // namespace tao::pq
