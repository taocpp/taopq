// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/table_reader.hpp>

#include <libpq-fe.h>

#include <cstring>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/transaction.hpp>
#include <tao/pq/result.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   namespace internal
   {
      class transaction_guard final
         : public subtransaction_base< parameter_text_traits >  // note: the traits are never used
      {
      public:
         explicit transaction_guard( const std::shared_ptr< connection >& connection )
            : subtransaction_base< parameter_text_traits >( connection )
         {}

         // transaction_guard( const transaction_guard& ) = delete;
         // transaction_guard( transaction_guard&& ) = delete;
         // void operator=( const transaction_guard& ) = delete;
         // void operator=( transaction_guard&& ) = delete;

      private:
         void v_commit() override {}
         void v_rollback() override {}
      };

   }  // namespace internal

   table_reader::table_reader( const std::shared_ptr< internal::transaction >& transaction, const std::string& statement )
      : m_previous( transaction ),
        m_transaction( std::make_shared< internal::transaction_guard >( transaction->m_connection ) ),
        m_buffer( nullptr, &PQfreemem )
   {
      result( PQexecParams( m_transaction->underlying_raw_ptr(), statement.c_str(), 0, nullptr, nullptr, nullptr, nullptr, 0 ), result::mode_t::expect_copy_out );
   }

   table_reader::~table_reader()
   {
      if( m_transaction ) {
         PQputCopyEnd( m_transaction->underlying_raw_ptr(), "cancelled in dtor" );
      }
   }

   bool table_reader::fetch_next()
   {
      char* buffer = nullptr;
      const auto result = PQgetCopyData( m_transaction->underlying_raw_ptr(), &buffer, 0 );
      m_buffer.reset( buffer );
      switch( result ) {
         case 0:
            // unreachable
            break;
         case -1:
            return false;
         case -2:
            throw std::runtime_error( "PQgetCopyData() failed: " + m_transaction->m_connection->error_message() );
      }
      return true;
   }

   const char* table_reader::get_data()
   {
      return static_cast< const char* >( m_buffer.get() );
   }

}  // namespace tao::pq
