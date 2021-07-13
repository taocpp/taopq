// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/table_writer.hpp>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/transaction.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   namespace
   {
      // TODO: This is inefficient, improve it!
      void append_escape( std::string& buffer, const char* data )
      {
         while( auto c = *data++ ) {
            switch( c ) {
               case '\b':
               case '\f':
               case '\n':
               case '\r':
               case '\t':
               case '\v':
               case '\\':
                  buffer += '\\';
                  [[fallthrough]];
               default:
                  buffer += c;
            }
         }
      }

   }  // namespace

   table_writer::table_writer( const std::shared_ptr< internal::transaction >& transaction, const std::string& statement )
      : m_transaction( transaction )
   {
      result( PQexecParams( transaction->m_connection->m_pgconn.get(), statement.c_str(), 0, nullptr, nullptr, nullptr, nullptr, 0 ), result::mode_t::expect_copy_in );
   }

   table_writer::~table_writer()
   {
      if( m_transaction ) {
         PQputCopyEnd( m_transaction->m_connection->m_pgconn.get(), "cancelled in dtor" );
      }
   }

   void table_writer::insert_raw( const std::string_view data )
   {
      const int r = PQputCopyData( m_transaction->m_connection->m_pgconn.get(), data.data(), static_cast< int >( data.size() ) );
      if( r != 1 ) {
         throw std::runtime_error( "PQputCopyData() failed: " + m_transaction->m_connection->error_message() );
      }
   }

   void table_writer::insert_values( const char* const values[], const std::size_t n_values )
   {
      std::string buffer;
      for( std::size_t n = 0; n < n_values; ++n ) {
         if( values[ n ] == nullptr ) {
            buffer += "\\N";
         }
         else {
            append_escape( buffer, values[ n ] );
         }
         buffer += '\t';
      }
      *buffer.rbegin() = '\n';
      insert_raw( buffer );
   }

   auto table_writer::finish() -> std::size_t
   {
      const auto c = m_transaction->m_connection;
      const int r = PQputCopyEnd( c->m_pgconn.get(), nullptr );
      if( r != 1 ) {
         throw std::runtime_error( "PQputCopyEnd() failed: " + c->error_message() );
      }
      m_transaction.reset();
      return result( PQgetResult( c->m_pgconn.get() ) ).rows_affected();
   }

}  // namespace tao::pq
