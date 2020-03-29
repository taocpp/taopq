// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_CONNECTION_HPP
#define TAO_PQ_CONNECTION_HPP

#include <memory>
#include <set>
#include <string>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/result.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   class connection_pool;
   class table_writer;

   namespace internal
   {
      struct deleter final
      {
         void operator()( PGconn* p ) const noexcept;
      };

   }  // namespace internal

   class connection final
      : public std::enable_shared_from_this< connection >
   {
   private:
      friend class connection_pool;
      friend class pq::transaction;
      friend class table_writer;

      const std::unique_ptr< PGconn, internal::deleter > m_pgconn;
      pq::transaction* m_current_transaction;
      std::set< std::string, std::less<> > m_prepared_statements;

      [[nodiscard]] auto error_message() const -> std::string;
      static void check_prepared_name( const std::string& name );
      [[nodiscard]] auto is_prepared( const char* name ) const noexcept -> bool;

      [[nodiscard]] auto execute_params( const char* statement,
                                         const int n_params,
                                         const Oid types[],
                                         const char* const values[],
                                         const int lengths[],
                                         const int formats[] ) -> result;

   public:
      [[nodiscard]] static auto create( const std::string& connection_info ) -> std::shared_ptr< connection >;

   private:
      // pass-key idiom
      class private_key
      {
         private_key() = default;
         friend class connection_pool;
         friend auto connection::create( const std::string& connection_info ) -> std::shared_ptr< connection >;
      };

   public:
      connection( const private_key& /*unused*/, const std::string& connection_info );

      connection( const connection& ) = delete;
      connection( connection&& ) = delete;
      void operator=( const connection& ) = delete;
      void operator=( connection&& ) = delete;

      ~connection() = default;

      [[nodiscard]] auto is_open() const noexcept -> bool;

      void prepare( const std::string& name, const std::string& statement );
      void deallocate( const std::string& name );

      [[nodiscard]] auto direct() -> std::shared_ptr< pq::transaction >;
      [[nodiscard]] auto transaction( const transaction::isolation_level il = transaction::isolation_level::default_isolation_level ) -> std::shared_ptr< pq::transaction >;

      template< template< typename... > class Traits = parameter_text_traits, typename... Ts >
      auto execute( Ts&&... ts )
      {
         return direct()->execute< Traits >( std::forward< Ts >( ts )... );
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
