// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#include <tao/postgres/result.hpp>

#include <cassert>
#include <cstring>
#include <stdexcept>

#include <tao/utility/printf.hpp>
#include <tao/utility/strtox.hpp>

namespace tao
{
  namespace postgres
  {
    void result::check_has_result_set() const
    {
      if( columns_ == 0 ) {
        throw std::runtime_error( "statement does not yield a result set" );
      }
    }

    result::result( ::PGresult* pgresult, const mode_t mode )
      : pgresult_( pgresult, &::PQclear ),
        columns_( ::PQnfields( pgresult ) ),
        rows_( ::PQntuples( pgresult ) )
    {
      const auto status = ::PQresultStatus( pgresult );
      switch( status ) {
      case PGRES_COMMAND_OK:
      case PGRES_TUPLES_OK:
        if( mode == mode_t::EXPECT_OK ) {
          return;
        }
        break;

      case PGRES_COPY_IN:
        if( mode == mode_t::EXPECT_COPY_IN ) {
          return;
        }
        break;

      case PGRES_EMPTY_QUERY:
        throw std::runtime_error( "empty query" );

      default:
        const std::string res_status = ::PQresStatus( status );
        const char* sql_state = ::PQresultErrorField( pgresult, PG_DIAG_SQLSTATE );
        const char* error_message = ::PQresultErrorMessage( pgresult );
        throw std::runtime_error( res_status + '/' + sql_state + ": " + error_message );
      }

      const std::string res_status = ::PQresStatus( status );
      throw std::runtime_error( "unexpected result: " + res_status );
    }

    bool result::has_rows_affected() const
    {
      const char* str = ::PQcmdTuples( pgresult_.get() );
      return str[ 0 ] != '\0';
    }

    std::size_t result::rows_affected() const
    {
      const char* str = ::PQcmdTuples( pgresult_.get() );
      if( str[ 0 ] == '\0' ) {
        throw std::runtime_error( "statement does not return affected rows" );
      }
      return utility::strtoul( str, 10 );
    }

    std::string result::name( const std::size_t column ) const
    {
      assert( column < columns_ );
      return ::PQfname( pgresult_.get(), column );
    }

    std::size_t result::index( const std::string& name ) const
    {
      const int column = ::PQfnumber( pgresult_.get(), name.c_str() );
      if( column < 0 ) {
        assert( column == -1 );
        check_has_result_set();
        throw std::runtime_error( "column not found: " + name );
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
      return rows_;
    }

    result::const_iterator result::begin() const
    {
      return row( *this, 0, 0, columns_ );
    }

    result::const_iterator result::end() const
    {
      return row( *this, size(), 0, columns_ );
    }

    bool result::is_null( const std::size_t row, const std::size_t column ) const
    {
      check_has_result_set();
      assert( row < rows_ );
      assert( column < columns_ );
      return ::PQgetisnull( pgresult_.get(), row, column );
    }

    const char* result::get( const std::size_t row, const std::size_t column ) const
    {
      if( is_null( row, column ) ) {
        throw std::runtime_error( utility::printf( "unexpected NULL value in row %lu column %lu = %s", row, column, name( column ).c_str() ) );
      }
      return ::PQgetvalue( pgresult_.get(), row, column );
    }

    row result::at( const std::size_t row ) const
    {
      check_has_result_set();
      if( !( row < rows_ ) ) {
        if( rows_ > 0 ) {
          throw std::out_of_range( utility::printf( "row %lu out of range (0-%lu)", row, rows_ - 1 ) );
        }
        else {
          throw std::out_of_range( utility::printf( "row %lu out of range, result is empty", row ) );
        }
      }
      return (*this)[ row ];
    }
  }
}
