// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/table_writer.hpp>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>
#include <tao/pq/result.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   table_writer::table_writer( const std::shared_ptr< transaction >& transaction, const std::string& statement )
      : m_transaction( transaction )
   {
      result( ::PQexecParams( transaction->m_connection->m_pgconn.get(), statement.c_str(), 0, nullptr, nullptr, nullptr, nullptr, 0 ), result::mode_t::expect_copy_in );
   }

   table_writer::~table_writer()
   {
      if( m_transaction ) {
         ::PQputCopyEnd( m_transaction->m_connection->m_pgconn.get(), "cancelled in dtor" );
      }
   }

   void table_writer::insert( const std::string& data )
   {
      const int r = ::PQputCopyData( m_transaction->m_connection->m_pgconn.get(), data.data(), static_cast< int >( data.size() ) );
      if( r != 1 ) {
         throw std::runtime_error( "::PQputCopyData() failed: " + m_transaction->m_connection->error_message() );
      }
   }

   std::size_t table_writer::finish()
   {
      const auto connection = m_transaction->m_connection;
      const int r = ::PQputCopyEnd( connection->m_pgconn.get(), nullptr );
      if( r != 1 ) {
         throw std::runtime_error( "::PQputCopyEnd() failed: " + connection->error_message() );
      }
      m_transaction.reset();
      return result( ::PQgetResult( connection->m_pgconn.get() ) ).rows_affected();
   }

}  // namespace tao::pq
