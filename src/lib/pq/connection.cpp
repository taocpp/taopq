// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/connection.hpp>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
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
               rollback_in_dtor();
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
            return !value.empty() && ( std::isdigit( static_cast< unsigned char >( value[ 0 ] ) ) == 0 ) && ( value.find_first_not_of( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ) == std::string_view::npos );
         }

      }  // namespace

   }  // namespace internal

   auto connection::escape_identifier( const std::string_view identifier ) const -> std::unique_ptr< char, decltype( &PQfreemem ) >
   {
      std::unique_ptr< char, decltype( &PQfreemem ) > buffer( PQescapeIdentifier( m_pgconn.get(), identifier.data(), identifier.size() ), &PQfreemem );
      if( !buffer ) {
         throw std::invalid_argument( error_message() );  // LCOV_EXCL_LINE
      }
      return buffer;
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
      const auto is_prepared = m_prepared_statements.contains( statement );
      if( m_log ) {
         if( is_prepared ) {
            if( m_log->connection.send_query_prepared ) {
               m_log->connection.send_query_prepared( *this, statement, n_params, values, lengths, formats );
            }
         }
         else {
            if( m_log->connection.send_query ) {
               m_log->connection.send_query( *this, statement, n_params, types, values, lengths, formats );
            }
         }
      }
      const auto result = is_prepared ?
                             PQsendQueryPrepared( m_pgconn.get(), statement, n_params, values, lengths, formats, 0 ) :
                             PQsendQueryParams( m_pgconn.get(), statement, n_params, types, values, lengths, formats, 0 );
      if( m_log ) {
         if( is_prepared ) {
            if( m_log->connection.send_query_prepared.result ) {
               m_log->connection.send_query_prepared.result( *this, result );
            }
         }
         else {
            if( m_log->connection.send_query.result ) {
               m_log->connection.send_query.result( *this, result );
            }
         }
      }
      if( result == 0 ) {
         throw pq::connection_error( error_message() );  // LCOV_EXCL_LINE
      }
   }

   void connection::wait( const bool wait_for_write, const std::chrono::steady_clock::time_point end )
   {
      if( m_log && m_log->connection.wait ) {
         m_log->connection.wait( *this, wait_for_write, end );
      }
      while( true ) {
         int timeout_ms = -1;
         if( m_timeout ) {
            timeout_ms = std::max( static_cast< int >( std::chrono::duration_cast< std::chrono::milliseconds >( end - std::chrono::steady_clock::now() ).count() ), 0 );
         }

         const auto so = socket();
         if( m_log && m_log->connection.poll ) {
            m_log->connection.poll( *this, so, wait_for_write, timeout_ms );
         }
         const auto status = m_poll( so, wait_for_write, timeout_ms );
         if( m_log && m_log->connection.poll.result ) {
            m_log->connection.poll.result( *this, so, status );
         }
         switch( status ) {
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
            throw pq::error( buffer );  // LCOV_EXCL_LINE
         }
      }
   }

   auto connection::get_result( const std::chrono::steady_clock::time_point end ) -> std::unique_ptr< PGresult, decltype( &PQclear ) >
   {
      if( m_log && m_log->connection.get_result ) {
         m_log->connection.get_result( *this, end );
      }
      bool wait_for_write = true;
      while( is_busy() ) {
         if( wait_for_write ) {
            wait_for_write = flush();
         }
         connection::wait( wait_for_write, end );
      }

      std::unique_ptr< PGresult, decltype( &PQclear ) > result( PQgetResult( m_pgconn.get() ), &PQclear );
      if( m_log && m_log->connection.get_result.result ) {
         m_log->connection.get_result.result( *this, result.get() );
      }
      handle_notifications();
      return result;
   }

   auto connection::get_fatal_error( const std::chrono::steady_clock::time_point end ) -> std::unique_ptr< PGresult, decltype( &PQclear ) >
   {
      auto result = connection::get_result( end );
      if( !result ) {
         throw std::runtime_error( "unable to obtain result" );
      }

      const auto status = PQresultStatus( result.get() );
      if( status != PGRES_FATAL_ERROR ) {
         throw std::runtime_error( std::format( "unexpected result status: {}", PQresStatus( status ) ) );
      }

      return result;
   }

   void connection::consume_empty_result( const std::chrono::steady_clock::time_point end )
   {
      if( const auto result = connection::get_result( end ) ) {
         const auto status = PQresultStatus( result.get() );
         throw std::runtime_error( std::format( "unexpected result status: {}", PQresStatus( status ) ) );
      }
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
               throw pq::error( std::format( "PQgetCopyData() failed: {}", error_message() ) );

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
               throw pq::error( std::format( "PQputCopyData() failed: {}", error_message() ) );

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
               throw pq::error( std::format( "PQputCopyEnd() failed: {}", connection::error_message() ) );

            default:
               TAO_PQ_INTERNAL_UNREACHABLE;
               // LCOV_EXCL_STOP
         }
      }
   }

   void connection::clear_copy_data( const std::chrono::steady_clock::time_point end )
   {
      char* ptr;
      while( connection::get_copy_data( ptr, end ) > 0 ) {
         PQfreemem( ptr );
      }
   }

   connection::connection( const private_key /*unused*/, const std::string& connection_info )
      : m_pgconn( PQconnectdb( connection_info.c_str() ), &PQfinish ),
        m_current_transaction( nullptr ),
        m_poll( internal::poll )
   {
      if( !is_open() ) {
         // note that we can not access the sqlstate after PQconnectdb(),
         // see https://stackoverflow.com/q/23349086/2073257
         throw pq::connection_error( error_message() );
      }

      if( PQsetnonblocking( m_pgconn.get(), 1 ) != 0 ) {
         throw pq::connection_error( error_message() );  // LCOV_EXCL_LINE
      }
   }

   auto connection::create( const std::string& connection_info ) -> std::shared_ptr< connection >
   {
      return std::make_shared< connection >( private_key(), connection_info );
   }

   auto connection::error_message() const -> const char*
   {
      return PQerrorMessage( m_pgconn.get() );
   }

   auto connection::notification_handler( const std::string_view channel ) const -> std::function< void( const char* payload ) >
   {
      const auto it = m_notification_handlers.find( channel );
      if( it != m_notification_handlers.end() ) {
         return it->second;
      }
      return {};
   }

   void connection::set_notification_handler( const std::string_view channel, const std::function< void( const char* payload ) >& handler )
   {
      m_notification_handlers[ std::string( channel ) ] = handler;
   }

   void connection::reset_notification_handler( const std::string_view channel ) noexcept
   {
      const auto it = m_notification_handlers.find( channel );
      if( it != m_notification_handlers.end() ) {
         m_notification_handlers.erase( it );
      }
   }

   auto connection::status() const noexcept -> connection_status
   {
      return static_cast< connection_status >( PQstatus( m_pgconn.get() ) );
   }

   auto connection::transaction_status() const noexcept -> pq::transaction_status
   {
      return static_cast< pq::transaction_status >( PQtransactionStatus( m_pgconn.get() ) );
   }

   auto connection::pipeline_status() const noexcept -> pq::pipeline_status
   {
      return static_cast< pq::pipeline_status >( PQpipelineStatus( m_pgconn.get() ) );
   }

   void connection::enter_pipeline_mode()
   {
      const auto result = PQenterPipelineMode( m_pgconn.get() );
      if( m_log && m_log->connection.enter_pipeline_mode.result ) {
         m_log->connection.enter_pipeline_mode.result( *this, result );
      }
      if( result == 0 ) {
         throw pq::connection_error( "unable to enter pipeline mode" );
      }
   }

   void connection::exit_pipeline_mode()
   {
      if( m_log && m_log->connection.exit_pipeline_mode ) {
         m_log->connection.exit_pipeline_mode( *this );
      }
      const auto result = PQexitPipelineMode( m_pgconn.get() );
      if( m_log && m_log->connection.exit_pipeline_mode.result ) {
         m_log->connection.exit_pipeline_mode.result( *this, result );
      }
      if( result == 0 ) {
         throw pq::connection_error( error_message() );
      }
   }

   void connection::pipeline_sync()
   {
      if( m_log && m_log->connection.pipeline_sync ) {
         m_log->connection.pipeline_sync( *this );
      }
      const auto result = PQpipelineSync( m_pgconn.get() );
      if( m_log && m_log->connection.pipeline_sync.result ) {
         m_log->connection.pipeline_sync.result( *this, result );
      }
      if( result == 0 ) {
         throw pq::connection_error( "unable to sync pipeline" );
      }
   }

   auto connection::is_busy() const noexcept -> bool
   {
      const auto result = PQisBusy( m_pgconn.get() );
      if( m_log && m_log->connection.is_busy.result ) {
         m_log->connection.is_busy.result( *this, result );
      }
      return result != 0;
   }

   auto connection::flush() -> bool
   {
      if( m_log && m_log->connection.flush ) {
         m_log->connection.flush( *this );
      }
      const auto result = PQflush( m_pgconn.get() );
      if( m_log && m_log->connection.flush.result ) {
         m_log->connection.flush.result( *this, result );
      }
      switch( result ) {
         case 0:
            return false;

         case 1:
            return true;

         default:
            throw pq::error( std::format( "PQflush() failed: {}", error_message() ) );
      }
   }

   void connection::consume_input()
   {
      if( m_log && m_log->connection.consume_input ) {
         m_log->connection.consume_input( *this );
      }
      const auto result = PQconsumeInput( m_pgconn.get() );
      if( m_log && m_log->connection.consume_input.result ) {
         m_log->connection.consume_input.result( *this, result );
      }
      if( result == 0 ) {
         throw pq::connection_error( error_message() );
      }
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

   auto connection::pipeline() -> std::shared_ptr< pq::pipeline >
   {
      return direct()->pipeline();
   }

   void connection::prepare( std::string name, const internal::zsv statement )
   {
      connection::check_prepared_name( name );
      const auto end = timeout_end();

      if( PQsendPrepare( m_pgconn.get(), name.c_str(), statement, 0, nullptr ) == 0 ) {
         throw pq::connection_error( error_message() );  // LCOV_EXCL_LINE
      }

      const auto result = connection::get_result( end );
      switch( PQresultStatus( result.get() ) ) {
         case PGRES_COMMAND_OK:
            connection::consume_empty_result( end );
            break;

         case PGRES_TUPLES_OK:
         case PGRES_EMPTY_QUERY:
         case PGRES_COPY_IN:
         case PGRES_COPY_OUT:
            TAO_PQ_INTERNAL_UNREACHABLE;  // LCOV_EXCL_LINE

         default:
            connection::consume_empty_result( end );
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
      connection::execute( std::format( "DEALLOCATE {}", connection::escape_identifier( name ).get() ) );
      m_prepared_statements.erase( it );
   }

   void connection::listen( const std::string_view channel )
   {
      connection::execute( std::format( "LISTEN {}", connection::escape_identifier( channel ).get() ) );
   }

   void connection::listen( const std::string_view channel, const std::function< void( const char* payload ) >& handler )
   {
      connection::set_notification_handler( channel, handler );
      connection::listen( channel );
   }

   void connection::unlisten( const std::string_view channel )
   {
      connection::execute( std::format( "UNLISTEN {}", connection::escape_identifier( channel ).get() ) );
   }

   void connection::notify( const std::string_view channel )
   {
      connection::execute( std::format( "NOTIFY {}", connection::escape_identifier( channel ).get() ) );
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
      consume_input();
      handle_notifications();
   }

   auto connection::socket() const -> int
   {
      const auto fd = PQsocket( m_pgconn.get() );
      if( fd < 0 ) {
         throw pq::error( "PQsocket(): unable to retrieve file descriptor" );  // LCOV_EXCL_LINE
      }
      return fd;
   }

   auto connection::password( const internal::zsv passwd, const internal::zsv user, const internal::zsv algorithm ) -> std::string
   {
      const std::unique_ptr< char, decltype( &PQfreemem ) > buffer( PQencryptPasswordConn( m_pgconn.get(), passwd, user, algorithm ), &PQfreemem );
      if( !buffer ) {
         throw std::invalid_argument( error_message() );  // LCOV_EXCL_LINE
      }
      return buffer.get();
   }

}  // namespace tao::pq
