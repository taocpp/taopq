// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/table_writer.hpp>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>
#include <tao/pq/exception.hpp>
#include <tao/pq/result.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   table_writer::~table_writer()
   {
      if( m_transaction ) {
         PQputCopyEnd( m_transaction->connection()->underlying_raw_ptr(), "cancel COPY FROM in dtor" );
      }
   }

   void table_writer::check_result()
   {
      auto result = m_transaction->connection()->get_result();
      switch( PQresultStatus( result.get() ) ) {
         case PGRES_COPY_IN:
            break;

         case PGRES_COPY_OUT:
            // TODO: How to cancel an unexpected PGRES_COPY_OUT?
            throw std::runtime_error( "unexpected COPY TO statement" );

         case PGRES_COMMAND_OK:
         case PGRES_TUPLES_OK:
            throw std::runtime_error( "expected COPY FROM statement" );

         case PGRES_EMPTY_QUERY:
            throw std::runtime_error( "unexpected empty query" );

         default:
            internal::throw_sqlstate( result.get() );
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
      const auto rows_affected = m_transaction->get_result().rows_affected();
      m_transaction.reset();
      m_previous.reset();
      return rows_affected;
   }

}  // namespace tao::pq
