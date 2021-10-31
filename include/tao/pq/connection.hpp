// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_CONNECTION_HPP
#define TAO_PQ_CONNECTION_HPP

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/access_mode.hpp>
#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/isolation_level.hpp>
#include <tao/pq/notification.hpp>
#include <tao/pq/oid.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   class connection_pool;
   class table_reader;
   class table_writer;

   class connection final
      : public std::enable_shared_from_this< connection >
   {
   private:
      friend class connection_pool;
      friend class table_reader;
      friend class table_writer;
      friend class transaction;

      const std::unique_ptr< PGconn, decltype( &PQfinish ) > m_pgconn;
      pq::transaction* m_current_transaction;
      std::set< std::string, std::less<> > m_prepared_statements;
      std::function< void( const notification& ) > m_notification_handler;
      std::map< std::string, std::function< void( const char* ) >, std::less<> > m_notification_handlers;

      [[nodiscard]] auto escape_identifier( const std::string_view identifier ) const -> std::string;

      static void check_prepared_name( const std::string_view name );
      [[nodiscard]] auto is_prepared( const std::string_view name ) const noexcept -> bool;

      void send_params( const char* statement,
                        const int n_params,
                        const Oid types[],
                        const char* const values[],
                        const int lengths[],
                        const int formats[] );

      [[nodiscard]] auto get_result() noexcept -> std::unique_ptr< PGresult, decltype( &PQclear ) >;

      // pass-key idiom
      class private_key final
      {
         private_key() = default;
         friend class connection;
         friend class connection_pool;
      };

   public:
      explicit connection( const private_key /*unused*/, const std::string& connection_info );

      connection( const connection& ) = delete;
      connection( connection&& ) = delete;
      void operator=( const connection& ) = delete;
      void operator=( connection&& ) = delete;

      ~connection() = default;

      [[nodiscard]] static auto create( const std::string& connection_info ) -> std::shared_ptr< connection >;

      [[nodiscard]] auto error_message() const -> std::string;

      [[nodiscard]] auto notification_handler() const -> std::function< void( const notification& ) >;
      [[nodiscard]] auto notification_handler( const std::string_view channel ) const -> std::function< void( const char* payload ) >;

      void set_notification_handler( const std::function< void( const notification& ) >& handler );
      void set_notification_handler( const std::string_view channel, const std::function< void( const char* payload ) >& handler );

      void reset_notification_handler() noexcept;
      void reset_notification_handler( const std::string_view channel ) noexcept;

      [[nodiscard]] auto is_open() const noexcept -> bool;

      [[nodiscard]] auto direct() -> std::shared_ptr< pq::transaction >;

      [[nodiscard]] auto transaction() -> std::shared_ptr< pq::transaction >;
      [[nodiscard]] auto transaction( const access_mode am, const isolation_level il = isolation_level::default_isolation_level ) -> std::shared_ptr< pq::transaction >;
      [[nodiscard]] auto transaction( const isolation_level il, const access_mode am = access_mode::default_access_mode ) -> std::shared_ptr< pq::transaction >;

      void prepare( const std::string& name, const std::string& statement );
      void deallocate( const std::string& name );

      template< typename... As >
      auto execute( const internal::zsv statement, As&&... as )
      {
         return direct()->execute( statement, std::forward< As >( as )... );
      }

      void listen( const std::string_view channel );
      void listen( const std::string_view channel, const std::function< void( const char* payload ) >& handler );
      void unlisten( const std::string_view channel );

      void notify( const std::string_view channel );
      void notify( const std::string_view channel, const std::string_view payload );

      void handle_notifications();
      void get_notifications();

      [[nodiscard]] auto underlying_raw_ptr() noexcept -> PGconn*
      {
         return m_pgconn.get();
      }

      [[nodiscard]] auto underlying_raw_ptr() const noexcept -> const PGconn*
      {
         return m_pgconn.get();
      }
   };

}  // namespace tao::pq

#endif
