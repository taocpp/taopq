// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/connection.hpp>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
#include <exception>
#include <format>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/access_mode.hpp>
#include <tao/pq/connection_status.hpp>
#include <tao/pq/exception.hpp>
#include <tao/pq/internal/poll.hpp>
#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/isolation_level.hpp>
#include <tao/pq/notification.hpp>
#include <tao/pq/oid.hpp>
#include <tao/pq/poll.hpp>
#include <tao/pq/result.hpp>
#include <tao/pq/transaction_status.hpp>

namespace tao::pq
{
   namespace internal
   {
      class transaction_base
         : public transaction
      {
      protected:
         explicit transaction_base( const std::shared_ptr< pq::connection >& connection )
            : transaction( connection )
         {
            if( current_transaction() != nullptr ) {
               throw std::logic_error( "invalid transaction order" );
            }
            current_transaction() = this;
         }

         ~transaction_base() override
         {
            if( m_connection ) {
               current_transaction() = nullptr;
            }
         }

         void v_reset() noexcept final
         {
            current_transaction() = nullptr;
            m_connection.reset();
         }

      public:
         transaction_base( const transaction_base& ) = delete;
         transaction_base( transaction_base&& ) = delete;
         void operator=( const transaction_base& ) = delete;
         void operator=( transaction_base&& ) = delete;
      };

      class autocommit_transaction final
         : public transaction_base
      {
      public:
         explicit autocommit_transaction( const std::shared_ptr< pq::connection >& connection )
            : transaction_base( connection )
         {}

      private:
         [[nodiscard]] auto v_is_direct() const noexcept -> bool override
         {
            return true;
         }

         void v_commit() override
         {}

         void v_rollback() override
         {}
      };

      namespace
      {
         [[nodiscard]] inline auto isolation_level_extension( const isolation_level il ) -> const char*
         {
            switch( il ) {
               case isolation_level::default_isolation_level:
                  return "";
               case isolation_level::serializable:
                  return " ISOLATION LEVEL SERIALIZABLE";
               case isolation_level::repeatable_read:
                  return " ISOLATION LEVEL REPEATABLE READ";
               case isolation_level::read_committed:
                  return " ISOLATION LEVEL READ COMMITTED";
               case isolation_level::read_uncommitted:
                  return " ISOLATION LEVEL READ UNCOMMITTED";
            }
            TAO_PQ_INTERNAL_UNREACHABLE;  // LCOV_EXCL_LINE
         }

         [[nodiscard]] inline auto access_mode_extension( const access_mode am ) -> const char*
         {
            switch( am ) {
               case access_mode::default_access_mode:
                  return "";
               case access_mode::read_write:
                  return " READ WRITE";
               case access_mode::read_only:
                  return " READ ONLY";
            }
            TAO_PQ_INTERNAL_UNREACHABLE;  // LCOV_EXCL_LINE
         }

      }  // namespace

      class top_level_transaction final
         : public transaction_base
      {
      public:
         top_level_transaction( const std::shared_ptr< pq::connection >& connection, const isolation_level il, const access_mode am )
            : transaction_base( connection )
         {
            this->execute( std::format( "START TRANSACTION{}{}", isolation_level_extension( il ), access_mode_extension( am ) ) );
         }

         ~top_level_transaction() override
         {
            if( m_connection && m_connection->attempt_rollback() ) {
               try {
                  rollback();
               }
               // LCOV_EXCL_START
               catch( const std::exception& ) {  // NOLINT(bugprone-empty-catch)
                  // TAO_LOG( WARNING, "unable to rollback transaction, swallowing exception: " + std::string( e.what() ) );
               }
               catch( ... ) {  // NOLINT(bugprone-empty-catch)
                  // TAO_LOG( WARNING, "unable to rollback transaction, swallowing unknown exception" );
               }
               // LCOV_EXCL_STOP
            }
         }

         top_level_transaction( const top_level_transaction& ) = delete;
         top_level_transaction( top_level_transaction&& ) = delete;
         void operator=( const top_level_transaction& ) = delete;
         void operator=( top_level_transaction&& ) = delete;

      private:
         [[nodiscard]] auto v_is_direct() const noexcept -> bool override
         {
            return false;
         }

         void v_commit() override
         {
            execute( "COMMIT TRANSACTION" );
         }

         void v_rollback() override
         {
            execute( "ROLLBACK TRANSACTION" );
         }
      };

      namespace
      {
         [[nodiscard]] constexpr auto is_identifier( const std::string_view value ) noexcept -> bool
         {
            return !value.empty() && ( value.find_first_not_of( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ) == std::string_view::npos ) && ( std::isdigit( value[ 0 ] ) == 0 );
         }

      }  // namespace

   }  // namespace internal

   auto connection::escape_identifier( const std::string_view identifier ) const -> std::string
   {
      const std::unique_ptr< char, decltype( &PQfreemem ) > buffer( PQescapeIdentifier( m_pgconn.get(), identifier.data(), identifier.size() ), &PQfreemem );
      if( !buffer ) {
         throw std::invalid_argument( PQerrorMessage( m_pgconn.get() ) );  // LCOV_EXCL_LINE
      }
      return buffer.get();
   }

   auto connection::attempt_rollback() const noexcept -> bool
   {
      switch( transaction_status() ) {
         // LCOV_EXCL_START
         case transaction_status::idle:
         case transaction_status::active:
            return false;
            // LCOV_EXCL_STOP

         case transaction_status::in_transaction:
         case transaction_status::error:
         case transaction_status::unknown:
            return true;
      }
      TAO_PQ_INTERNAL_UNREACHABLE;  // LCOV_EXCL_LINE
   }

   void connection::check_prepared_name( const std::string_view name )
   {
      if( !internal::is_identifier( name ) ) {
         throw std::invalid_argument( std::format( "invalid prepared statement name: {}", name ) );
      }
   }

   void connection::send_params( const char* statement,
                                 const int n_params,
                                 const Oid types[],
                                 const char* const values[],
                                 const int lengths[],
                                 const int formats[] )
   {
      const auto result = m_prepared_statements.contains( statement ) ?
                             PQsendQueryPrepared( m_pgconn.get(), statement, n_params, values, lengths, formats, 0 ) :
                             PQsendQueryParams( m_pgconn.get(), statement, n_params, types, values, lengths, formats, 0 );
      if( result == 0 ) {
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ) );  // LCOV_EXCL_LINE
      }
   }

   auto connection::timeout_end( const std::chrono::steady_clock::time_point start ) const noexcept -> std::chrono::steady_clock::time_point
   {
      return m_timeout ? ( start + *m_timeout ) : start;
   }

   void connection::wait( const bool wait_for_write, const std::chrono::steady_clock::time_point end )
   {
      while( true ) {
         int timeout_ms = -1;
         if( m_timeout ) {
            timeout_ms = std::max( static_cast< int >( std::chrono::duration_cast< std::chrono::milliseconds >( end - std::chrono::steady_clock::now() ).count() ), 0 );
         }

         switch( m_poll( socket(), wait_for_write, timeout_ms ) ) {
            case poll::status::timeout:
               m_pgconn.reset();
               throw timeout_reached( "timeout reached" );

            case poll::status::readable:
               get_notifications();
               return;

               // LCOV_EXCL_START
            case poll::status::writable:
               return;

            case poll::status::again:
               break;

            default:
               TAO_PQ_INTERNAL_UNREACHABLE;
               // LCOV_EXCL_STOP
         }
      }
   }

   void connection::cancel()
   {
      const std::unique_ptr< PGcancel, decltype( &PQfreeCancel ) > p( PQgetCancel( m_pgconn.get() ), &PQfreeCancel );
      if( p ) {
         char buffer[ 256 ];
         if( PQcancel( p.get(), buffer, sizeof( buffer ) ) == 0 ) {
            throw std::runtime_error( buffer );  // LCOV_EXCL_LINE
         }
      }
   }

   auto connection::get_result( const std::chrono::steady_clock::time_point end ) -> std::unique_ptr< PGresult, decltype( &PQclear ) >
   {
      bool wait_for_write = true;
      while( PQisBusy( m_pgconn.get() ) != 0 ) {
         if( wait_for_write ) {
            switch( PQflush( m_pgconn.get() ) ) {
               case 0:
                  wait_for_write = false;
                  break;

                  // LCOV_EXCL_START
               case 1:
                  break;

               default:
                  throw std::runtime_error( "PQflush() failed: " + error_message() );
                  // LCOV_EXCL_STOP
            }
         }
         connection::wait( wait_for_write, end );
      }

      std::unique_ptr< PGresult, decltype( &PQclear ) > result( PQgetResult( m_pgconn.get() ), &PQclear );
      handle_notifications();
      return result;
   }

   auto connection::get_copy_data( char*& buffer, const std::chrono::steady_clock::time_point end ) -> std::size_t
   {
      while( true ) {
         const auto result = PQgetCopyData( m_pgconn.get(), &buffer, 1 );
         if( result > 0 ) {
            return static_cast< std::size_t >( result );
         }
         switch( result ) {
            case 0:
               connection::wait( false, end );
               break;

            case -1:
               return 0;

               // LCOV_EXCL_START
            case -2:
               throw std::runtime_error( "PQgetCopyData() failed: " + error_message() );

            default:
               TAO_PQ_INTERNAL_UNREACHABLE;
               // LCOV_EXCL_STOP
         }
      }
   }

   auto connection::get_copy_data( char*& buffer ) -> std::size_t
   {
      return connection::get_copy_data( buffer, timeout_end() );
   }

   void connection::put_copy_data( const char* buffer, const std::size_t size )
   {
      const auto end = timeout_end();
      while( true ) {
         switch( PQputCopyData( m_pgconn.get(), buffer, static_cast< int >( size ) ) ) {
            case 1:
               return;

               // LCOV_EXCL_START
            case 0:
               connection::wait( true, end );
               break;

            case -1:
               throw std::runtime_error( "PQputCopyData() failed: " + error_message() );

            default:
               TAO_PQ_INTERNAL_UNREACHABLE;
               // LCOV_EXCL_STOP
         }
      }
   }

   void connection::put_copy_end( const char* error_message )
   {
      const auto end = timeout_end();
      while( true ) {
         switch( PQputCopyEnd( m_pgconn.get(), error_message ) ) {
            case 1:
               return;

               // LCOV_EXCL_START
            case 0:
               connection::wait( true, end );
               break;

            case -1:
               throw std::runtime_error( "PQputCopyEnd() failed: " + connection::error_message() );

            default:
               TAO_PQ_INTERNAL_UNREACHABLE;
               // LCOV_EXCL_STOP
         }
      }
   }

   void connection::clear_results( const std::chrono::steady_clock::time_point end )
   {
      while( connection::get_result( end ) ) {
      }
   }

   void connection::clear_copy_data( const std::chrono::steady_clock::time_point end )
   {
      char* ptr;
      while( connection::get_copy_data( ptr, end ) > 0 ) {
         PQfreemem( ptr );
      }
   }

   connection::connection( const private_key /*unused*/, const std::string& connection_info, std::function< poll::callback > poll_cb )
      : m_pgconn( PQconnectdb( connection_info.c_str() ), &PQfinish ),
        m_current_transaction( nullptr ),
        m_poll( std::move( poll_cb ) )
   {
      if( !is_open() ) {
         // note that we can not access the sqlstate after PQconnectdb(),
         // see https://stackoverflow.com/q/23349086/2073257
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ) );
      }

      if( PQsetnonblocking( m_pgconn.get(), 1 ) != 0 ) {
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ) );  // LCOV_EXCL_LINE
      }
   }

   auto connection::create( const std::string& connection_info, std::function< poll::callback > poll_cb ) -> std::shared_ptr< connection >
   {
      return std::make_shared< connection >( private_key(), connection_info, std::move( poll_cb ) );
   }

   auto connection::error_message() const -> std::string
   {
      return PQerrorMessage( m_pgconn.get() );
   }

   auto connection::poll_callback() const noexcept -> const std::function< poll::callback >&
   {
      return m_poll;
   }

   void connection::set_poll_callback( std::function< poll::callback > poll_cb ) noexcept
   {
      m_poll = std::move( poll_cb );
   }

   void connection::reset_poll_callback()
   {
      m_poll = internal::poll;
   }

   auto connection::notification_handler() const -> std::function< void( const notification& ) >
   {
      return m_notification_handler;
   }

   auto connection::notification_handler( const std::string_view channel ) const -> std::function< void( const char* payload ) >
   {
      const auto it = m_notification_handlers.find( channel );
      if( it != m_notification_handlers.end() ) {
         return it->second;
      }
      return {};
   }

   void connection::set_notification_handler( const std::function< void( const notification& ) >& handler )
   {
      m_notification_handler = handler;
   }

   void connection::set_notification_handler( const std::string_view channel, const std::function< void( const char* payload ) >& handler )
   {
      m_notification_handlers[ std::string( channel ) ] = handler;
   }

   void connection::reset_notification_handler() noexcept
   {
      m_notification_handler = nullptr;
   }

   void connection::reset_notification_handler( const std::string_view channel ) noexcept
   {
      m_notification_handlers.erase( std::string( channel ) );
   }

   auto connection::status() const noexcept -> connection_status
   {
      return static_cast< connection_status >( PQstatus( m_pgconn.get() ) );
   }

   auto connection::transaction_status() const noexcept -> pq::transaction_status
   {
      return static_cast< pq::transaction_status >( PQtransactionStatus( m_pgconn.get() ) );
   }

   auto connection::direct() -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< internal::autocommit_transaction >( shared_from_this() );
   }

   auto connection::transaction() -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< internal::top_level_transaction >( shared_from_this(), isolation_level::default_isolation_level, access_mode::default_access_mode );
   }

   auto connection::transaction( const access_mode am, const isolation_level il ) -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< internal::top_level_transaction >( shared_from_this(), il, am );
   }

   auto connection::transaction( const isolation_level il, const access_mode am ) -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< internal::top_level_transaction >( shared_from_this(), il, am );
   }

   void connection::prepare( std::string name, const internal::zsv statement )
   {
      connection::check_prepared_name( name );
      const auto end = timeout_end();
      if( PQsendPrepare( m_pgconn.get(), name.c_str(), statement, 0, nullptr ) == 0 ) {
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ) );  // LCOV_EXCL_LINE
      }
      auto result = connection::get_result( end );
      switch( PQresultStatus( result.get() ) ) {
         case PGRES_COMMAND_OK:
            connection::clear_results( end );
            break;

         case PGRES_TUPLES_OK:
         case PGRES_EMPTY_QUERY:
         case PGRES_COPY_IN:
         case PGRES_COPY_OUT:
            TAO_PQ_INTERNAL_UNREACHABLE;  // LCOV_EXCL_LINE

         default:
            connection::clear_results( end );
            internal::throw_sqlstate( result.get() );
      }
      m_prepared_statements.insert( std::move( name ) );
   }

   void connection::deallocate( const std::string_view name )
   {
      connection::check_prepared_name( name );
      const auto it = m_prepared_statements.find( name );
      if( it == m_prepared_statements.end() ) {
         throw std::runtime_error( std::format( "prepared statement not found: {}", name ) );
      }
      connection::execute( "DEALLOCATE " + connection::escape_identifier( name ) );
      m_prepared_statements.erase( it );
   }

   void connection::listen( const std::string_view channel )
   {
      connection::execute( "LISTEN " + connection::escape_identifier( channel ) );
   }

   void connection::listen( const std::string_view channel, const std::function< void( const char* payload ) >& handler )
   {
      connection::set_notification_handler( channel, handler );
      connection::listen( channel );
   }

   void connection::unlisten( const std::string_view channel )
   {
      connection::execute( "UNLISTEN " + connection::escape_identifier( channel ) );
   }

   void connection::notify( const std::string_view channel )
   {
      connection::execute( "NOTIFY " + connection::escape_identifier( channel ) );
   }

   void connection::notify( const std::string_view channel, const std::string_view payload )
   {
      connection::execute( "SELECT pg_notify( $1, $2 )", channel, payload );
   }

   void connection::handle_notifications()
   {
      while( PGnotify* pgnotify = PQnotifies( m_pgconn.get() ) ) {
         const notification notify( pgnotify );
         if( m_notification_handler ) {
            m_notification_handler( notify );
         }
         const auto it = m_notification_handlers.find( notify.channel() );
         if( it != m_notification_handlers.end() ) {
            it->second( notify.payload() );
         }
      }
   }

   void connection::get_notifications()
   {
      if( PQconsumeInput( m_pgconn.get() ) == 0 ) {
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ) );
      }
      handle_notifications();
   }

   auto connection::socket() const -> int
   {
      const auto fd = PQsocket( m_pgconn.get() );
      if( fd < 0 ) {
         throw std::runtime_error( "PQsocket(): unable to retrieve file descriptor" );  // LCOV_EXCL_LINE
      }
      return fd;
   }

   void connection::set_timeout( const std::chrono::milliseconds timeout )
   {
      m_timeout = timeout;
   }

   void connection::reset_timeout() noexcept
   {
      m_timeout = std::nullopt;
   }

   auto connection::password( const internal::zsv passwd, const internal::zsv user, const internal::zsv algorithm ) -> std::string
   {
      const std::unique_ptr< char, decltype( &PQfreemem ) > buffer( PQencryptPasswordConn( m_pgconn.get(), passwd, user, algorithm ), &PQfreemem );
      if( !buffer ) {
         throw std::invalid_argument( PQerrorMessage( m_pgconn.get() ) );  // LCOV_EXCL_LINE
      }
      return buffer.get();
   }

}  // namespace tao::pq
