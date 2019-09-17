// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_CONNECTION_HPP
#define TAO_PQ_CONNECTION_HPP

#include <memory>
#include <set>
#include <string>
#include <utility>

#include <tao/pq/result.hpp>
#include <tao/pq/transaction.hpp>

// forward-declare libpq structures
struct pg_conn;
typedef struct pg_conn PGconn;

namespace tao::pq
{
   class connection_pool;
   class table_writer;

   namespace internal
   {
      struct deleter final
      {
         void operator()( ::PGconn* p ) const noexcept;
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

      [[nodiscard]] std::string error_message() const;
      void check_prepared_name( const std::string& name ) const;
      [[nodiscard]] bool is_prepared( const char* name ) const noexcept;

      [[nodiscard]] result execute_params( const char* statement,
                                           const int n_params,
                                           const char* const param_values[],
                                           const int param_lengths[],
                                           const int param_formats[] );

   public:
      [[nodiscard]] static std::shared_ptr< connection > create( const std::string& connect_info );

   private:
      // pass-key idiom
      class private_key
      {
         private_key() = default;
         friend class connection_pool;
         friend std::shared_ptr< connection > connection::create( const std::string& connect_info );
      };

   public:
      connection( const private_key&, const std::string& connect_info );

      connection( const connection& ) = delete;
      void operator=( const connection& ) = delete;

      [[nodiscard]] bool is_open() const noexcept;

      void prepare( const std::string& name, const std::string& statement );
      void deallocate( const std::string& name );

      [[nodiscard]] std::shared_ptr< pq::transaction > direct();
      [[nodiscard]] std::shared_ptr< pq::transaction > transaction( const transaction::isolation_level il = transaction::isolation_level::default_isolation_level );

      template< typename... Ts >
      result execute( Ts&&... ts )
      {
         return direct()->execute( std::forward< Ts >( ts )... );
      }

      // make sure you include libpq-fe.h before accessing the raw pointer
      [[nodiscard]] ::PGconn* underlying_raw_ptr() noexcept
      {
         return m_pgconn.get();
      }

      // make sure you include libpq-fe.h before accessing the raw pointer
      [[nodiscard]] const ::PGconn* underlying_raw_ptr() const noexcept
      {
         return m_pgconn.get();
      }
   };

}  // namespace tao::pq

#endif
