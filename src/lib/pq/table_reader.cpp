// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/table_reader.hpp>

#include <libpq-fe.h>

#include <cassert>
#include <cstring>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   auto table_reader::get_raw_data() -> std::string_view
   {
      char* buffer = nullptr;
      const auto result = PQgetCopyData( m_transaction->connection()->underlying_raw_ptr(), &buffer, 0 );
      m_buffer.reset( buffer );
      if( result > 0 ) {
         return { static_cast< const char* >( buffer ), static_cast< std::size_t >( result ) };
      }
      switch( result ) {
         case 0:                 // LCOV_EXCL_LINE
            TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
         case -1: {
            (void)pq::result( PQgetResult( m_transaction->connection()->underlying_raw_ptr() ) );
            m_transaction.reset();
            m_previous.reset();
            return {};
         }
         case -2:
            throw std::runtime_error( "PQgetCopyData() failed: " + m_transaction->connection()->error_message() );
      }
      TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
   }

   auto table_reader::parse_data() noexcept -> bool
   {
      m_data.clear();
      char* read = m_buffer.get();
      if( read == nullptr ) {
         return false;
      }
      char* write = read;
      char* begin = write;
      while( auto* pos = std::strpbrk( read, "\t\\\n" ) ) {
         if( const auto prefix_size = pos - read ) {
            std::memmove( write, read, prefix_size );
            write += prefix_size;
         }
         switch( *pos ) {
            case '\t':
               m_data.emplace_back( begin );
               *write++ = '\0';
               begin = write = read = ++pos;
               break;

            case '\\':
               read = pos + 1;
               switch( *read++ ) {
                  case 'N':
                     assert( write == begin );
                     m_data.emplace_back( nullptr );
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
               m_data.emplace_back( begin );
               *write++ = '\0';
               assert( m_data.size() == columns() );
               return true;

            default:                // LCOV_EXCL_LINE
               TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
         }
      }
      TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
   }

   auto table_reader::begin() -> table_reader::const_iterator
   {
      (void)get_row();
      return table_row( *this, 0, columns() );
   }

   auto table_reader::end() noexcept -> table_reader::const_iterator
   {
      return table_row( *this, 0, 0 );
   }

}  // namespace tao::pq
