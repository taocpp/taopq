// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/row.hpp>

#include <cassert>
#include <cstddef>
#include <format>
#include <stdexcept>
#include <string>

#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   void row::ensure_column( const std::size_t column ) const
   {
      if( column >= m_columns ) {
         throw std::out_of_range( std::format( "column {} out of range (0-{})", column, m_columns - 1 ) );
      }
   }

   auto row::slice( const std::size_t offset, const std::size_t in_columns ) const -> row
   {
      assert( m_result );
      if( in_columns == 0 ) {
         throw std::invalid_argument( "slice requires at least one column" );
      }
      if( offset + in_columns > m_columns ) {
         throw std::out_of_range( std::format( "slice ({}-{}) out of range (0-{})", offset, offset + in_columns - 1, m_columns - 1 ) );
      }
      return { *m_result, m_row, m_offset + offset, in_columns };
   }

   auto row::name( const std::size_t column ) const -> std::string
   {
      assert( m_result );
      return m_result->name( m_offset + column );
   }

   auto row::index( const internal::zsv in_name ) const -> std::size_t
   {
      assert( m_result );
      const std::size_t n = m_result->index( in_name );
      if( n >= m_offset ) {
         if( n - m_offset < m_columns ) {
            return n - m_offset;
         }
      }
      else {
         const std::string adapted_name = m_result->name( n );
         for( std::size_t pos = 0; pos < m_columns; ++pos ) {
            if( name( pos ) == adapted_name ) {
               return pos;
            }
         }
      }
      throw std::out_of_range( std::format( "column not found: {}", static_cast< const char* >( in_name ) ) );
   }

   auto row::begin() const -> row::const_iterator
   {
      return const_iterator( field( *this, m_offset ) );
   }

   auto row::end() const -> row::const_iterator
   {
      return const_iterator( field( *this, m_offset + m_columns ) );
   }

   auto row::is_null( const std::size_t column ) const -> bool
   {
      ensure_column( column );
      assert( m_result );
      return m_result->is_null( m_row, m_offset + column );
   }

   auto row::get( const std::size_t column ) const -> const char*
   {
      ensure_column( column );
      assert( m_result );
      return m_result->get( m_row, m_offset + column );
   }

   auto row::at( const std::size_t column ) const -> field
   {
      ensure_column( column );
      return { *this, m_offset + column };
   }

}  // namespace tao::pq
