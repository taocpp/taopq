// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/table_reader.hpp>

#include <libpq-fe.h>

#include <cassert>
#include <cstring>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/transaction.hpp>
#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/result.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   table_reader::table_reader( const std::shared_ptr< internal::transaction >& transaction, const std::string& statement )
      : m_previous( transaction ),
        m_transaction( std::make_shared< internal::transaction_guard >( transaction->m_connection ) ),
        m_buffer( nullptr, &PQfreemem )
   {
      result( PQexecParams( m_transaction->underlying_raw_ptr(), statement.c_str(), 0, nullptr, nullptr, nullptr, nullptr, 0 ), result::mode_t::expect_copy_out );
   }

   auto table_reader::get_raw_data() -> std::string_view
   {
      char* buffer = nullptr;
      const auto result = PQgetCopyData( m_transaction->underlying_raw_ptr(), &buffer, 0 );
      m_buffer.reset( buffer );
      if( result > 0 ) {
         return { static_cast< const char* >( buffer ), static_cast< std::size_t >( result ) };
      }
      switch( result ) {
         case 0:                 // LCOV_EXCL_LINE
            TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
         case -1: {
            (void)pq::result( PQgetResult( m_transaction->underlying_raw_ptr() ) );
            m_transaction.reset();
            m_previous.reset();
            return {};
         }
         case -2:
            throw std::runtime_error( "PQgetCopyData() failed: " + m_transaction->m_connection->error_message() );
      }
      TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
   }

   auto table_reader::parse_data() noexcept -> bool
   {
      char* read = m_buffer.get();
      if( read == nullptr ) {
         return false;
      }
      char* write = read;
      char* begin = write;
      m_fields.clear();
      while( auto* pos = std::strpbrk( read, "\t\\\n" ) ) {
         if( const auto prefix_size = pos - read ) {
            std::memmove( write, read, prefix_size );
            write += prefix_size;
         }
         switch( *pos ) {
            case '\t':
               m_fields.emplace_back( begin, write - begin );
               *write++ = '\0';
               begin = write = read = ++pos;
               break;

            case '\\':
               read = pos + 1;
               switch( *read++ ) {
                  case 'N':
                     assert( write == begin );
                     m_fields.emplace_back();
                     switch( *read ) {
                        case '\t':
                           begin = write = ++read;
                           break;

                        case '\n':
                           return true;

                        default:                // LCOV_EXCL_LINE
                           TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
                     }
                     break;

                  case 'b':
                     *write++ = '\b';
                     break;

                  case 'f':
                     *write++ = '\f';
                     break;

                  case 'n':
                     *write++ = '\n';
                     break;

                  case 'r':
                     *write++ = '\r';
                     break;

                  case 't':
                     *write++ = '\t';
                     break;

                  case 'v':
                     *write++ = '\v';
                     break;

                  case '\\':
                     *write++ = '\\';
                     break;

                  default:                // LCOV_EXCL_LINE
                     TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
               }
               break;

            case '\n':
               m_fields.emplace_back( begin, write - begin );
               *write++ = '\0';
               return true;

            default:                // LCOV_EXCL_LINE
               TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
         }
      }
      TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
   }

   auto table_reader::get_row() -> bool
   {
      (void)get_raw_data();
      return parse_data();
   }

}  // namespace tao::pq
