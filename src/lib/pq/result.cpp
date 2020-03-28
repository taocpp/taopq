// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <cassert>
#include <cstring>
#include <stdexcept>

#include <libpq-fe.h>

#include <tao/pq/result.hpp>

#include <tao/pq/internal/printf.hpp>
#include <tao/pq/internal/strtox.hpp>

namespace tao::pq
{
   void result::check_has_result_set() const
   {
      if( m_columns == 0 ) {
         throw std::logic_error( "statement does not yield a result set" );
      }
   }

   void result::check_row( const std::size_t row ) const
   {
      check_has_result_set();
      if( !( row < m_rows ) ) {
         if( m_rows == 0 ) {
            throw std::out_of_range( internal::printf( "row %zu out of range, result is empty", row ) );
         }
         throw std::out_of_range( internal::printf( "row %zu out of range (0-%zu)", row, m_rows - 1 ) );
      }
   }

   result::result( PGresult* pgresult, const mode_t mode )
      : m_pgresult( pgresult, &PQclear ),
        m_columns( PQnfields( pgresult ) ),
        m_rows( PQntuples( pgresult ) )
   {
      const auto status = PQresultStatus( pgresult );
      switch( status ) {
         case PGRES_COMMAND_OK:
         case PGRES_TUPLES_OK:
            if( mode == mode_t::expect_ok ) {
               return;
            }
            break;

         case PGRES_COPY_IN:
            if( mode == mode_t::expect_copy_in ) {
               return;
            }
            break;

         case PGRES_EMPTY_QUERY:
            throw std::runtime_error( "empty query" );

         default:
            const std::string res_status = PQresStatus( status );
            const char* sql_state = PQresultErrorField( pgresult, PG_DIAG_SQLSTATE );
            const char* error_message = PQresultErrorMessage( pgresult );
            throw std::runtime_error( res_status + '/' + ( ( sql_state != nullptr ) ? sql_state : "?" ) + ": " + error_message );
      }

      const std::string res_status = PQresStatus( status );
      throw std::runtime_error( "unexpected result: " + res_status );
   }

   bool result::has_rows_affected() const
   {
      const char* str = PQcmdTuples( m_pgresult.get() );
      return str[ 0 ] != '\0';
   }

   std::size_t result::rows_affected() const
   {
      const char* str = PQcmdTuples( m_pgresult.get() );
      if( str[ 0 ] == '\0' ) {
         throw std::logic_error( "statement does not return affected rows" );
      }
      return internal::strtoul( str, 10 );
   }

   std::string result::name( const std::size_t column ) const
   {
      if( column >= m_columns ) {
         throw std::out_of_range( internal::printf( "column %zu out of range (0-%zu)", column, m_columns - 1 ) );
      }
      return PQfname( m_pgresult.get(), static_cast< int >( column ) );
   }

   std::size_t result::index( const std::string& in_name ) const
   {
      const int column = PQfnumber( m_pgresult.get(), in_name.c_str() );
      if( column < 0 ) {
         assert( column == -1 );
         check_has_result_set();
         throw std::out_of_range( "column not found: " + in_name );
      }
      return column;
   }

   bool result::empty() const
   {
      return size() == 0;
   }

   std::size_t result::size() const
   {
      check_has_result_set();
      return m_rows;
   }

   result::const_iterator result::begin() const
   {
      return row( *this, 0, 0, m_columns );
   }

   result::const_iterator result::end() const
   {
      return row( *this, size(), 0, m_columns );
   }

   bool result::is_null( const std::size_t row, const std::size_t column ) const
   {
      check_row( row );
      if( column >= m_columns ) {
         throw std::out_of_range( internal::printf( "column %zu out of range (0-%zu)", column, m_columns - 1 ) );
      }
      return PQgetisnull( m_pgresult.get(), static_cast< int >( row ), static_cast< int >( column ) ) != 0;
   }

   const char* result::get( const std::size_t row, const std::size_t column ) const
   {
      if( is_null( row, column ) ) {
         throw std::runtime_error( internal::printf( "unexpected NULL value in row %zu column %zu = %s", row, column, name( column ).c_str() ) );
      }
      return PQgetvalue( m_pgresult.get(), static_cast< int >( row ), static_cast< int >( column ) );
   }

   row result::at( const std::size_t row ) const
   {
      check_row( row );
      return ( *this )[ row ];
   }

}  // namespace tao::pq
