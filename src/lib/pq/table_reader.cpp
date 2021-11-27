// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/table_reader.hpp>

#include <chrono>
#include <cstring>
#include <tuple>

#include <tao/pq/connection.hpp>
#include <tao/pq/exception.hpp>
#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   void table_reader::check_result()
   {
      const auto start = std::chrono::steady_clock::now();
      const auto end = m_transaction->connection()->timeout_end( start );
      auto result = m_transaction->connection()->get_result( end );
      switch( PQresultStatus( result.get() ) ) {
         case PGRES_COPY_OUT:
            m_columns = PQnfields( result.get() );
            break;

         case PGRES_COPY_IN:
            std::ignore = m_transaction->get_result( start );
            TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE

         case PGRES_COMMAND_OK:
         case PGRES_TUPLES_OK:
            m_transaction->connection()->clear_results( end );
            throw std::runtime_error( "expected COPY TO statement" );

         case PGRES_EMPTY_QUERY:
            m_transaction->connection()->clear_results( end );
            throw std::runtime_error( "unexpected empty query" );

         default:
            m_transaction->connection()->clear_results( end );
            internal::throw_sqlstate( result.get() );
      }
   }

   auto table_reader::get_raw_data() -> std::string_view
   {
      char* buffer = nullptr;
      const auto size = m_transaction->connection()->get_copy_data( buffer );
      m_buffer.reset( buffer );

      if( size > 0 ) {
         return { static_cast< const char* >( buffer ), size };
      }

      const auto start = std::chrono::steady_clock::now();
      const auto end = m_transaction->connection()->timeout() ? ( start + *m_transaction->connection()->timeout() ) : start;

      std::ignore = pq::result( m_transaction->connection()->get_result( end ).release() );
      m_transaction.reset();
      m_previous.reset();
      return {};
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
      std::ignore = get_row();
      return table_row( *this, 0, columns() );
   }

   auto table_reader::end() noexcept -> table_reader::const_iterator
   {
      return table_row( *this, 0, 0 );
   }

}  // namespace tao::pq
