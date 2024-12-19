// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/transaction_base.hpp>

#include <chrono>
#include <cstdio>
#include <format>
#include <memory>
#include <stdexcept>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   transaction_base::transaction_base( const std::shared_ptr< pq::connection >& connection ) noexcept  // NOLINT(modernize-pass-by-value)
      : m_connection( connection )
   {}

   auto transaction_base::current_transaction() const noexcept -> transaction_base*&
   {
      return m_connection->m_current_transaction;
   }

   void transaction_base::check_current_transaction() const
   {
      if( !m_connection || this != current_transaction() ) {
         throw std::logic_error( "invalid transaction order" );
      }
   }

   void transaction_base::send_params( const char* statement,
                                       const int n_params,
                                       const Oid types[],
                                       const char* const values[],
                                       const int lengths[],
                                       const int formats[] )
   {
      check_current_transaction();
      m_connection->send_params( statement, n_params, types, values, lengths, formats );
   }

   void transaction_base::set_single_row_mode()
   {
      check_current_transaction();
      if( PQsetSingleRowMode( m_connection->underlying_raw_ptr() ) == 0 ) {
         throw std::runtime_error( "unable to switch to single row mode" );
      }
   }

#if defined( LIBPQ_HAS_CHUNK_MODE )
   void transaction_base::set_chunk_mode( const int rows )
   {
      check_current_transaction();
      if( PQsetChunkedRowsMode( m_connection->underlying_raw_ptr(), rows ) == 0 ) {
         throw std::runtime_error( "unable to switch to chunk mode" );
      }
   }
#endif

   auto transaction_base::get_result( const std::chrono::steady_clock::time_point start ) -> result
   {
      check_current_transaction();
      const auto end = m_connection->timeout_end( start );

      auto result = m_connection->get_result( end );
      if( !result ) {
         throw std::runtime_error( "unable to obtain result" );
      }

      switch( PQresultStatus( result.get() ) ) {
         case PGRES_COPY_IN:
            m_connection->put_copy_end( "unexpected COPY FROM statement" );
            result = m_connection->get_fatal_error( end );
            break;

         case PGRES_COPY_OUT:
            m_connection->cancel();
            m_connection->clear_copy_data( end );
            std::ignore = m_connection->get_fatal_error( end );
            m_connection->consume_empty_result( end );
            throw std::runtime_error( "unexpected COPY TO statement" );

         case PGRES_SINGLE_TUPLE:
#if defined( LIBPQ_HAS_CHUNK_MODE )
         case PGRES_TUPLES_CHUNK:
#endif
            return pq::result( result.release() );

         default:;
      }

      m_connection->consume_empty_result( end );
      return pq::result( result.release() );
   }

   void transaction_base::consume_pipeline_sync( const std::chrono::steady_clock::time_point start )
   {
      check_current_transaction();
      const auto end = m_connection->timeout_end( start );

      const auto result = m_connection->get_result( end );
      if( !result ) {
         throw std::runtime_error( "unable to obtain result" );
      }

      const auto status = PQresultStatus( result.get() );
      if( status != PGRES_PIPELINE_SYNC ) {
         throw std::runtime_error( std::format( "unexpected result status: {}", PQresStatus( status ) ) );
      }
   }

}  // namespace tao::pq
