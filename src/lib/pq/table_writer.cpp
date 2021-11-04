// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/table_writer.hpp>

#include <chrono>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>
#include <tao/pq/exception.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   table_writer::~table_writer()
   {
      if( m_transaction ) {
         try {
            (void)m_transaction->get_result();
         }
         catch( ... ) {
         }
      }
   }

   void table_writer::check_result()
   {
      const auto start = std::chrono::steady_clock::now();
      auto result = m_transaction->connection()->get_result( start );
      switch( PQresultStatus( result.get() ) ) {
         case PGRES_COPY_IN:
            break;

         case PGRES_COPY_OUT:
            // TODO: How to cancel an unexpected PGRES_COPY_OUT?
            while( m_transaction->connection()->get_result( start ) ) {
            }
            throw std::runtime_error( "unexpected COPY TO statement" );

         case PGRES_COMMAND_OK:
         case PGRES_TUPLES_OK:
            while( m_transaction->connection()->get_result( start ) ) {
            }
            throw std::runtime_error( "expected COPY FROM statement" );

         case PGRES_EMPTY_QUERY:
            while( m_transaction->connection()->get_result( start ) ) {
            }
            throw std::runtime_error( "unexpected empty query" );

         default:
            while( m_transaction->connection()->get_result( start ) ) {
            }
            internal::throw_sqlstate( result.get() );
      }
   }

   void table_writer::insert_raw( const std::string_view data )
   {
      m_transaction->connection()->put_copy_data( data.data(), data.size() );
   }

   auto table_writer::commit() -> std::size_t
   {
      m_transaction->connection()->put_copy_end();
      const auto rows_affected = m_transaction->get_result().rows_affected();
      m_transaction.reset();
      m_previous.reset();
      return rows_affected;
   }

}  // namespace tao::pq
