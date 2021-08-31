// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_CONNECTION_HPP
#define TAO_PQ_CONNECTION_HPP

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/access_mode.hpp>
#include <tao/pq/isolation_level.hpp>
#include <tao/pq/oid.hpp>
#include <tao/pq/result.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   class large_object;
   class table_reader;
   class table_writer;

   namespace internal
   {
      struct deleter final
      {
         void operator()( PGconn* p ) const noexcept
         {
            PQfinish( p );
         }
      };

   }  // namespace internal

   class connection final
      : public std::enable_shared_from_this< connection >
   {
   private:
      friend class large_object;
      friend class table_reader;
      friend class table_writer;
      friend class transaction;

      const std::unique_ptr< PGconn, internal::deleter > m_pgconn;
      pq::transaction* m_current_transaction;
      std::set< std::string, std::less<> > m_prepared_statements;

      [[nodiscard]] auto error_message() const -> std::string;
      [[nodiscard]] auto escape_identifier( const std::string_view identifier ) const -> std::string;

      static void check_prepared_name( const std::string_view name );
      [[nodiscard]] auto is_prepared( const std::string_view name ) const noexcept -> bool;

      [[nodiscard]] auto execute_params( const char* statement,
                                         const int n_params,
                                         const Oid types[],
                                         const char* const values[],
                                         const int lengths[],
                                         const int formats[] ) -> result;

   public:
      explicit connection( const std::string& connection_info );

      connection( const connection& ) = delete;
      connection( connection&& ) = delete;
      void operator=( const connection& ) = delete;
      void operator=( connection&& ) = delete;

      ~connection() = default;

      [[nodiscard]] static auto create( const std::string& connection_info ) -> std::shared_ptr< connection >;

      [[nodiscard]] auto is_open() const noexcept -> bool;

      [[nodiscard]] auto direct() -> std::shared_ptr< pq::transaction >;

      [[nodiscard]] auto transaction() -> std::shared_ptr< pq::transaction >;
      [[nodiscard]] auto transaction( const access_mode am, const isolation_level il = isolation_level::default_isolation_level ) -> std::shared_ptr< pq::transaction >;
      [[nodiscard]] auto transaction( const isolation_level il, const access_mode am = access_mode::default_access_mode ) -> std::shared_ptr< pq::transaction >;

      void prepare( const std::string& name, const std::string& statement );
      void deallocate( const std::string& name );

      template< typename... Ts >
      auto execute( Ts&&... ts )
      {
         return direct()->execute( std::forward< Ts >( ts )... );
      }

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
