// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <cassert>
#include <charconv>
#include <cstring>
#include <stdexcept>

#include <libpq-fe.h>

#include <tao/pq/exception.hpp>
#include <tao/pq/result.hpp>

#include <tao/pq/internal/from_chars.hpp>
#include <tao/pq/internal/printf.hpp>

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

         case PGRES_COPY_OUT:
            if( mode == mode_t::expect_copy_out ) {
               return;
            }
            break;

         case PGRES_EMPTY_QUERY:
            throw std::runtime_error( "empty query" );

         default:
            const char* error_message = PQresultErrorMessage( pgresult );
            const std::string_view sql_state = PQresultErrorField( pgresult, PG_DIAG_SQLSTATE );
            switch( sql_state[ 0 ] ) {
               case '0':
                  switch( sql_state[ 1 ] ) {
                     case '8':
                        throw broken_connection( error_message, sql_state );

                     case 'A':
                        throw feature_not_supported( error_message, sql_state );
                  }
                  break;

               case '2':
                  switch( sql_state[ 1 ] ) {
                     case '2':
                        throw data_exception( error_message, sql_state );

                     case '3':
                        if( sql_state == "23001" ) {
                           throw restrict_violation( error_message, sql_state );
                        }
                        if( sql_state == "23502" ) {
                           throw not_null_violation( error_message, sql_state );
                        }
                        if( sql_state == "23503" ) {
                           throw foreign_key_violation( error_message, sql_state );
                        }
                        if( sql_state == "23505" ) {
                           throw unique_violation( error_message, sql_state );
                        }
                        if( sql_state == "23514" ) {
                           throw check_violation( error_message, sql_state );
                        }
                        if( sql_state == "23P01" ) {
                           throw exclusion_violation( error_message, sql_state );
                        }
                        throw integrity_constraint_violation( error_message, sql_state );

                     case '4':
                        throw invalid_cursor_state( error_message, sql_state );

                     case '5':
                        throw invalid_transaction_state( error_message, sql_state );
                  }
                  break;

               case '4':
                  switch( sql_state[ 1 ] ) {
                     case '0':
                        if( sql_state == "40001" ) {
                           throw serialization_failure( error_message, sql_state );
                        }
                        if( sql_state == "40002" ) {
                           throw transaction_integrity_constraint_violation( error_message, sql_state );
                        }
                        if( sql_state == "40003" ) {
                           throw statement_completion_unknown( error_message, sql_state );
                        }
                        if( sql_state == "40P01" ) {
                           throw deadlock_detected( error_message, sql_state );
                        }
                        throw transaction_rollback( error_message, sql_state );

                     case '2':
                        if( sql_state == "42501" ) {
                           throw insufficient_privilege( error_message, sql_state );
                        }
                        if( sql_state == "42601" ) {
                           throw syntax_error( error_message, sql_state );
                        }
                        if( sql_state == "42703" ) {
                           throw undefined_column( error_message, sql_state );
                        }
                        if( sql_state == "42883" ) {
                           throw undefined_function( error_message, sql_state );
                        }
                        if( sql_state == "42P01" ) {
                           throw undefined_table( error_message, sql_state );
                        }
                        throw syntax_error_or_access_rule_violation( error_message, sql_state );
                  }
                  break;

               case '5':
                  switch( sql_state[ 1 ] ) {
                     case '3':
                        if( sql_state == "53100" ) {
                           throw disk_full( error_message, sql_state );
                        }
                        if( sql_state == "53200" ) {
                           throw out_of_memory( error_message, sql_state );
                        }
                        if( sql_state == "53300" ) {
                           throw too_many_connections( error_message, sql_state );
                        }
                        if( sql_state == "53400" ) {
                           throw configuration_limit_exceeded( error_message, sql_state );
                        }
                        throw insufficient_resources( error_message, sql_state );

                     case '7':
                        if( sql_state == "57014" ) {
                           throw query_canceled( error_message, sql_state );
                        }
                        if( sql_state == "57P01" ) {
                           throw admin_shutdown( error_message, sql_state );
                        }
                        if( sql_state == "57P02" ) {
                           throw crash_shutdown( error_message, sql_state );
                        }
                        if( sql_state == "57P03" ) {
                           throw cannot_connect_now( error_message, sql_state );
                        }
                        if( sql_state == "57P04" ) {
                           throw database_dropped( error_message, sql_state );
                        }
                        throw operator_intervention( error_message, sql_state );
                  }
                  break;
            }
            throw sql_error( error_message, sql_state );
      }

      const std::string res_status = PQresStatus( status );
      throw std::runtime_error( "unexpected result: " + res_status );
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
         throw std::out_of_range( internal::printf( "column %zu out of range (0-%zu)", column, m_columns - 1 ) );
      }
      return PQfname( m_pgresult.get(), static_cast< int >( column ) );
   }

   auto result::index( const internal::zsv in_name ) const -> std::size_t
   {
      const int column = PQfnumber( m_pgresult.get(), in_name );
      if( column < 0 ) {
         assert( column == -1 );
         check_has_result_set();
         throw std::out_of_range( "column not found: " + std::string( in_name ) );
      }
      return column;
   }

   auto result::empty() const -> bool
   {
      return size() == 0;
   }

   auto result::size() const -> std::size_t
   {
      check_has_result_set();
      return m_rows;
   }

   auto result::begin() const -> result::const_iterator
   {
      check_has_result_set();
      return const_iterator( row( *this, 0, 0, m_columns ) );
   }

   auto result::end() const -> result::const_iterator
   {
      return const_iterator( row( *this, size(), 0, m_columns ) );
   }

   auto result::is_null( const std::size_t row, const std::size_t column ) const -> bool
   {
      check_row( row );
      if( column >= m_columns ) {
         throw std::out_of_range( internal::printf( "column %zu out of range (0-%zu)", column, m_columns - 1 ) );
      }
      return PQgetisnull( m_pgresult.get(), static_cast< int >( row ), static_cast< int >( column ) ) != 0;
   }

   auto result::get( const std::size_t row, const std::size_t column ) const -> const char*
   {
      if( is_null( row, column ) ) {
         throw std::runtime_error( internal::printf( "unexpected NULL value in row %zu column %zu = %s", row, column, name( column ).c_str() ) );
      }
      return PQgetvalue( m_pgresult.get(), static_cast< int >( row ), static_cast< int >( column ) );
   }

   auto result::at( const std::size_t row ) const -> pq::row
   {
      check_row( row );
      return ( *this )[ row ];
   }

}  // namespace tao::pq
