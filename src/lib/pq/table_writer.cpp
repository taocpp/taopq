// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/table_writer.hpp>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>
#include <tao/pq/result.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   table_writer::~table_writer()
   {
      if( m_transaction ) {
         PQputCopyEnd( m_transaction->connection()->underlying_raw_ptr(), "cancelled in dtor" );
      }
   }

   void table_writer::insert_raw( const std::string_view data )
   {
      const int r = PQputCopyData( m_transaction->connection()->underlying_raw_ptr(), data.data(), static_cast< int >( data.size() ) );
      if( r != 1 ) {
         throw std::runtime_error( "PQputCopyData() failed: " + m_transaction->connection()->error_message() );
      }
   }

   auto table_writer::commit() -> std::size_t
   {
      const int r = PQputCopyEnd( m_transaction->connection()->underlying_raw_ptr(), nullptr );
      if( r != 1 ) {
         throw std::runtime_error( "PQputCopyEnd() failed: " + m_transaction->connection()->error_message() );
      }
      const auto rows_affected = result( PQgetResult( m_transaction->connection()->underlying_raw_ptr() ) ).rows_affected();
      m_transaction->connection()->handle_notifications();
      m_transaction.reset();
      m_previous.reset();
      return rows_affected;
   }

}  // namespace tao::pq
