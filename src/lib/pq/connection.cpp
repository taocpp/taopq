// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/connection.hpp>

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#if defined( _WIN32 )
#include <winsock2.h>
#else
#include <poll.h>
#endif

#include <tao/pq/exception.hpp>
#include <tao/pq/internal/printf.hpp>
#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/notification.hpp>
#include <tao/pq/oid.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   namespace
   {
      [[nodiscard, maybe_unused]] auto errno_result_to_string( const int e, char* buffer, int result ) -> std::string
      {
         if( result == 0 ) {
            return buffer;
         }
         return internal::printf( "unknown error code %d", e );
      }

      [[nodiscard, maybe_unused]] auto errno_result_to_string( const int /*unused*/, char* /*unused*/, char* result ) -> std::string
      {
         return result;
      }

      [[nodiscard]] auto errno_to_string( const int e ) -> std::string
      {
         char buffer[ 256 ];
#if defined( _WIN32 )
         return errno_result_to_string( e, buffer, strerror_s( buffer, e ) );
#else
         return errno_result_to_string( e, buffer, strerror_r( e, buffer, sizeof( buffer ) ) );
#endif
      }

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
         TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
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
         TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
      }

      class top_level_transaction final
         : public transaction_base
      {
      public:
         explicit top_level_transaction( const std::shared_ptr< pq::connection >& connection, const isolation_level il, const access_mode am )
            : transaction_base( connection )
         {
            this->execute( std::string( "START TRANSACTION" ) + isolation_level_extension( il ) + access_mode_extension( am ) );
         }

         ~top_level_transaction() override
         {
            if( m_connection && m_connection->is_open() ) {
               try {
                  rollback();
               }
               // LCOV_EXCL_START
               catch( const std::exception& ) {
                  // TAO_LOG( WARNING, "unable to rollback transaction, swallowing exception: " + std::string( e.what() ) );
               }
               catch( ... ) {
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

      [[nodiscard]] constexpr auto is_identifier( const std::string_view value ) noexcept -> bool
      {
         return !value.empty() && ( value.find_first_not_of( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ) == std::string_view::npos ) && ( std::isdigit( value[ 0 ] ) == 0 );
      }

   }  // namespace

   auto connection::escape_identifier( const std::string_view identifier ) const -> std::string
   {
      const std::unique_ptr< char, decltype( &PQfreemem ) > buffer( PQescapeIdentifier( m_pgconn.get(), identifier.data(), identifier.size() ), &PQfreemem );
      if( !buffer ) {
         throw std::invalid_argument( PQerrorMessage( m_pgconn.get() ) );  // LCOV_EXCL_LINE
      }
      return buffer.get();
   }

   void connection::check_prepared_name( const std::string_view name )
   {
      if( !pq::is_identifier( name ) ) {
         throw std::invalid_argument( "invalid prepared statement name" );
      }
   }

   auto connection::is_prepared( const std::string_view name ) const noexcept -> bool
   {
      return m_prepared_statements.find( name ) != m_prepared_statements.end();
   }

   void connection::send_params( const char* statement,
                                 const int n_params,
                                 const Oid types[],
                                 const char* const values[],
                                 const int lengths[],
                                 const int formats[] )
   {
      const auto result = is_prepared( statement ) ?
                             PQsendQueryPrepared( m_pgconn.get(), statement, n_params, values, lengths, formats, 0 ) :
                             PQsendQueryParams( m_pgconn.get(), statement, n_params, types, values, lengths, formats, 0 );
      if( result == 0 ) {
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ), "08000" );
      }
   }

   void connection::wait( const bool wait_for_write, const std::chrono::steady_clock::time_point start )
   {
      const auto end = m_timeout ? ( start + *m_timeout ) : start;
      const short events = POLLIN | ( wait_for_write ? POLLOUT : 0 );
      while( true ) {
         int timeout = -1;
         if( m_timeout ) {
            timeout = std::chrono::duration_cast< std::chrono::milliseconds >( end - std::chrono::steady_clock::now() ).count();
            if( timeout < 0 ) {
               timeout = 0;
            }
         }

#if defined( _WIN32 )

         WSAPOLLFD pfd = { static_cast< SOCKET >( socket() ), events, 0 };
         const auto result = WSAPoll( &pfd, 1, timeout );
         switch( result ) {
            case 0:
               throw timeout_reached( "timeout reached" );

            case 1:
               if( ( pfd.revents & events ) == 0 ) {
                  throw std::runtime_error( internal::printf( "WSAPoll() failed, events %hd, revents %hd", events, pfd.revents ) );
               }
               if( ( pfd.revents & POLLIN ) != 0 ) {
                  get_notifications();
               }
               return;

            case SOCKET_ERROR:
               break;

            default:                // LCOV_EXCL_LINE
               TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
         }

         const int e = WSAGetLastError();
         throw std::runtime_error( "WSAPoll() failed: " + errno_to_string( e ) );

#else

         pollfd pfd = { socket(), events, 0 };
         errno = 0;
         const auto result = poll( &pfd, 1, timeout );
         switch( result ) {
            case 0:
               throw timeout_reached( "timeout reached" );

            case 1:
               if( ( pfd.revents & events ) == 0 ) {
                  throw std::runtime_error( internal::printf( "poll() failed, events %hd, revents %hd", events, pfd.revents ) );
               }
               if( ( pfd.revents & POLLIN ) != 0 ) {
                  get_notifications();
               }
               return;

            case -1:
               break;

            default:                // LCOV_EXCL_LINE
               TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
         }

         const int e = errno;
         if( ( e != EINTR ) && ( e != EAGAIN ) ) {
            throw std::runtime_error( "poll() failed: " + errno_to_string( e ) );
         }

#endif
      }
   }

   auto connection::get_result( const std::chrono::steady_clock::time_point start ) -> std::unique_ptr< PGresult, decltype( &PQclear ) >
   {
      bool wait_for_write = true;
      while( PQisBusy( m_pgconn.get() ) != 0 ) {
         if( wait_for_write ) {
            switch( PQflush( m_pgconn.get() ) ) {
               case 0:
                  wait_for_write = false;
                  break;

               case 1:
                  break;

               default:
                  throw std::runtime_error( "PQflush() failed: " + error_message() );
            }
         }
         wait( wait_for_write, start );
      }

      std::unique_ptr< PGresult, decltype( &PQclear ) > result( PQgetResult( m_pgconn.get() ), &PQclear );
      handle_notifications();
      return result;
   }

   auto connection::get_copy_data( char*& buffer ) -> std::size_t
   {
      const auto start = std::chrono::steady_clock::now();
      while( true ) {
         const auto result = PQgetCopyData( m_pgconn.get(), &buffer, 1 );
         if( result > 0 ) {
            return static_cast< std::size_t >( result );
         }
         switch( result ) {
            case 0:
               wait( false, start );
               break;

            case -1:
               return 0;

               // LCOV_EXCL_START
            case -2:
               throw std::runtime_error( "PQgetCopyData() failed: " + error_message() );

            default:
               TAO_PQ_UNREACHABLE;
               // LCOV_EXCL_END
         }
      }
   }

   void connection::put_copy_data( const char* buffer, const std::size_t size )
   {
      const auto start = std::chrono::steady_clock::now();
      while( true ) {
         switch( PQputCopyData( m_pgconn.get(), buffer, static_cast< int >( size ) ) ) {
            case 0:
               wait( true, start );
               break;

            case 1:
               return;

               // LCOV_EXCL_START
            case -1:
               throw std::runtime_error( "PQputCopyData() failed: " + error_message() );

            default:
               TAO_PQ_UNREACHABLE;
               // LCOV_EXCL_END
         }
      }
   }

   void connection::put_copy_end( const char* error_message )
   {
      const auto start = std::chrono::steady_clock::now();
      while( true ) {
         switch( PQputCopyEnd( m_pgconn.get(), error_message ) ) {
            case 0: {
               wait( true, start );
               break;
            }

            case 1:
               return;

               // LCOV_EXCL_START
            case -1:
               throw std::runtime_error( "PQputCopyEnd() failed: " + connection::error_message() );

            default:
               TAO_PQ_UNREACHABLE;
               // LCOV_EXCL_END
         }
      }
   }

   connection::connection( const private_key /*unused*/, const std::string& connection_info )
      : m_pgconn( PQconnectdb( connection_info.c_str() ), &PQfinish ),
        m_current_transaction( nullptr )
   {
      if( !is_open() ) {
         // note that we can not access the sqlstate after PQconnectdb(),
         // see https://stackoverflow.com/q/23349086/2073257
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ), "08000" );
      }

      if( PQsetnonblocking( m_pgconn.get(), 1 ) != 0 ) {
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ), "08000" );
      }
   }

   auto connection::create( const std::string& connection_info ) -> std::shared_ptr< connection >
   {
      return std::make_shared< connection >( private_key(), connection_info );
   }

   auto connection::error_message() const -> std::string
   {
      return PQerrorMessage( m_pgconn.get() );
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

   auto connection::is_open() const noexcept -> bool
   {
      return PQstatus( m_pgconn.get() ) == CONNECTION_OK;
   }

   auto connection::direct() -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< autocommit_transaction >( shared_from_this() );
   }

   auto connection::transaction() -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< top_level_transaction >( shared_from_this(), isolation_level::default_isolation_level, access_mode::default_access_mode );
   }

   auto connection::transaction( const access_mode am, const isolation_level il ) -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< top_level_transaction >( shared_from_this(), il, am );
   }

   auto connection::transaction( const isolation_level il, const access_mode am ) -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< top_level_transaction >( shared_from_this(), il, am );
   }

   void connection::prepare( const std::string& name, const std::string& statement )
   {
      const auto start = std::chrono::steady_clock::now();
      connection::check_prepared_name( name );
      if( PQsendPrepare( m_pgconn.get(), name.c_str(), statement.c_str(), 0, nullptr ) == 0 ) {
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ), "08000" );
      }
      auto result = get_result( start );
      switch( PQresultStatus( result.get() ) ) {
         case PGRES_COMMAND_OK:
            while( get_result( start ) ) {
            }
            break;

         case PGRES_TUPLES_OK:
         case PGRES_EMPTY_QUERY:
         case PGRES_COPY_IN:
         case PGRES_COPY_OUT:
            TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE

         default:
            while( get_result( start ) ) {
            }
            internal::throw_sqlstate( result.get() );
      }
      m_prepared_statements.insert( name );
   }

   void connection::deallocate( const std::string& name )
   {
      connection::check_prepared_name( name );
      if( !connection::is_prepared( name ) ) {
         throw std::runtime_error( "prepared statement not found: " + name );
      }
      connection::execute( "DEALLOCATE " + escape_identifier( name ) );
      m_prepared_statements.erase( name );
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
         throw pq::connection_error( PQerrorMessage( m_pgconn.get() ), "08000" );
      }
      handle_notifications();
   }

   auto connection::socket() const -> int
   {
      const auto fd = PQsocket( m_pgconn.get() );
      if( fd < 0 ) {
         throw std::runtime_error( "PQsocket(): unable to retrieve file descriptor" );
      }
      return fd;
   }

}  // namespace tao::pq
