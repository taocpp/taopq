// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/result.hpp>

#include <cassert>
#include <cstddef>
#include <format>
#include <stdexcept>
#include <string>

#include <libpq-fe.h>

#include <tao/pq/exception.hpp>
#include <tao/pq/internal/from_chars.hpp>
#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/internal/zsv.hpp>

namespace tao::pq
{
   void result::check_row( const std::size_t row ) const
   {
      assert( m_columns != 0 );
      if( !( row < m_rows ) ) {
         if( m_rows == 0 ) {
            throw std::out_of_range( std::format( "row {} out of range, result is empty", row ) );
         }
         throw std::out_of_range( std::format( "row {} out of range (0-{})", row, m_rows - 1 ) );
      }
   }

   result::result( PGresult* pgresult )
      : m_pgresult( pgresult, &PQclear ),
        m_columns( PQnfields( pgresult ) ),
        m_rows( PQntuples( pgresult ) )
   {
      switch( PQresultStatus( pgresult ) ) {
         case PGRES_COMMAND_OK:
         case PGRES_TUPLES_OK:
         case PGRES_SINGLE_TUPLE:
#if defined( LIBPQ_HAS_CHUNK_MODE )
         case PGRES_TUPLES_CHUNK:
#endif
         case PGRES_PIPELINE_SYNC:
         case PGRES_PIPELINE_ABORTED:
            return;

         case PGRES_EMPTY_QUERY:
            throw std::runtime_error( "unexpected empty query" );

         case PGRES_COPY_IN:
         case PGRES_COPY_OUT:
            TAO_PQ_INTERNAL_UNREACHABLE;  // LCOV_EXCL_LINE

         default:
            internal::throw_sqlstate( pgresult );
      }
   }

   auto result::status() const noexcept -> result_status
   {
      return static_cast< pq::result_status >( PQresultStatus( m_pgresult.get() ) );
   }

   auto result::has_rows_affected() const noexcept -> bool
   {
      const char* str = PQcmdTuples( m_pgresult.get() );
      return str[ 0 ] != '\0';
   }

   auto result::rows_affected() const -> std::size_t
   {
      const char* str = PQcmdTuples( m_pgresult.get() );
      if( str[ 0 ] == '\0' ) {
         throw std::logic_error( "statement does not return affected rows" );
      }
      return internal::from_chars< std::size_t >( str );
   }

   auto result::name( const std::size_t column ) const -> std::string
   {
      if( column >= m_columns ) {
         throw std::out_of_range( std::format( "column {} out of range (0-{})", column, m_columns - 1 ) );
      }
      return PQfname( m_pgresult.get(), static_cast< int >( column ) );
   }

   auto result::index( const internal::zsv in_name ) const -> std::size_t
   {
      assert( m_columns != 0 );
      const int column = PQfnumber( m_pgresult.get(), in_name );
      if( column < 0 ) {
         assert( column == -1 );
         throw std::out_of_range( std::format( "column '{}' not found", in_name.value ) );
      }
      return column;
   }

   auto result::begin() const noexcept -> result::const_iterator
   {
      assert( m_columns != 0 );
      return const_iterator( row( *this, 0, 0, m_columns ) );
   }

   auto result::end() const noexcept -> result::const_iterator
   {
      return const_iterator( row( *this, size(), 0, m_columns ) );
   }

   auto result::is_null( const std::size_t row, const std::size_t column ) const -> bool
   {
      check_row( row );
      if( column >= m_columns ) {
         throw std::out_of_range( std::format( "column {} out of range (0-{})", column, m_columns - 1 ) );
      }
      return PQgetisnull( m_pgresult.get(), static_cast< int >( row ), static_cast< int >( column ) ) != 0;
   }

   auto result::get( const std::size_t row, const std::size_t column ) const -> const char*
   {
      if( is_null( row, column ) ) {
         throw std::runtime_error( std::format( "unexpected NULL value in row {} column {}/'{}'", row, column, name( column ) ) );
      }
      return PQgetvalue( m_pgresult.get(), static_cast< int >( row ), static_cast< int >( column ) );
   }

   auto result::at( const std::size_t row ) const -> pq::row
   {
      check_row( row );
      return ( *this )[ row ];
   }

}  // namespace tao::pq
