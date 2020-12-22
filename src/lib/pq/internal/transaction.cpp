// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <stdexcept>

#include <tao/pq/internal/connection.hpp>
#include <tao/pq/internal/transaction.hpp>

namespace tao::pq::internal
{
   transaction::transaction( const std::shared_ptr< internal::connection >& connection )  // NOLINT(modernize-pass-by-value)
      : m_connection( connection )
   {}

   transaction::~transaction() = default;

   auto transaction::current_transaction() const noexcept -> transaction*&
   {
      return m_connection->m_current_transaction;
   }

   void transaction::check_current_transaction() const
   {
      if( !m_connection || this != current_transaction() ) {
         throw std::logic_error( "invalid transaction order" );
      }
   }

   auto transaction::execute_params( const char* statement,
                                     const int n_params,
                                     const Oid types[],
                                     const char* const values[],
                                     const int lengths[],
                                     const int formats[] ) -> result
   {
      check_current_transaction();
      return m_connection->execute_params( statement, n_params, types, values, lengths, formats );
   }

   auto transaction::underlying_raw_ptr() const noexcept -> PGconn*
   {
      return m_connection->underlying_raw_ptr();
   }

   void transaction::commit()
   {
      check_current_transaction();
      try {
         v_commit();
      }
      // LCOV_EXCL_START
      catch( ... ) {
         v_reset();
         throw;
      }
      // LCOV_EXCL_STOP
      v_reset();
   }

   void transaction::rollback()
   {
      check_current_transaction();
      try {
         v_rollback();
      }
      // LCOV_EXCL_START
      catch( ... ) {
         v_reset();
         throw;
      }
      // LCOV_EXCL_STOP
      v_reset();
   }

}  // namespace tao::pq::internal
