// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <cassert>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <string_view>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>

namespace tao::pq::internal
{
   namespace
   {
      [[nodiscard]] constexpr auto is_identifier( const std::string_view value ) noexcept -> bool
      {
         return !value.empty() && ( value.find_first_not_of( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ) == std::string_view::npos ) && ( std::isdigit( value[ 0 ] ) == 0 );
      }

   }  // namespace

   auto connection::error_message() const -> std::string
   {
      const char* message = PQerrorMessage( m_pgconn.get() );
      assert( message );
      const std::size_t size = std::strlen( message );
      assert( size > 0 );
      assert( message[ size - 1 ] == '\n' );
      return std::string( message, size - 1 );
   }

   void connection::check_prepared_name( const std::string& name )
   {
      if( !is_identifier( name ) ) {
         throw std::invalid_argument( "invalid prepared statement name" );
      }
   }

   auto connection::is_prepared( const char* name ) const noexcept -> bool
   {
      return m_prepared_statements.find( name ) != m_prepared_statements.end();
   }

   auto connection::execute_params( const char* statement,
                                    const int n_params,
                                    const Oid types[],
                                    const char* const values[],
                                    const int lengths[],
                                    const int formats[] ) -> result
   {
      if( is_prepared( statement ) ) {
         return result( PQexecPrepared( m_pgconn.get(), statement, n_params, values, lengths, formats, 0 ) );
      }
      return result( PQexecParams( m_pgconn.get(), statement, n_params, types, values, lengths, formats, 0 ) );
   }

   connection::connection( const std::string& connection_info )
      : m_pgconn( PQconnectdb( connection_info.c_str() ), deleter() ),
        m_current_transaction( nullptr )
   {
      if( !is_open() ) {
         throw std::runtime_error( "connection failed: " + error_message() );
      }
      const auto protocol_version = PQprotocolVersion( m_pgconn.get() );
      if( protocol_version < 3 ) {
         throw std::runtime_error( "protocol version 3 required" );  // LCOV_EXCL_LINE
      }
      // TODO: check server version
   }

   auto connection::is_open() const noexcept -> bool
   {
      return PQstatus( m_pgconn.get() ) == CONNECTION_OK;
   }

   void connection::prepare( const std::string& name, const std::string& statement )
   {
      check_prepared_name( name );
      result( PQprepare( m_pgconn.get(), name.c_str(), statement.c_str(), 0, nullptr ) );  // NOLINT(bugprone-unused-raii)
      m_prepared_statements.insert( name );
   }

   void connection::deallocate( const std::string& name )
   {
      check_prepared_name( name );
      if( !is_prepared( name.c_str() ) ) {
         throw std::runtime_error( "prepared statement name not found: " + name );
      }
      // TODO: this->execute( "DEALLOCATE " + name );
      m_prepared_statements.erase( name );
   }

}  // namespace tao::pq::internal
