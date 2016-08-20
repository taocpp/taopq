// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_POSTGRES_CONNECTION_HPP
#define TAOCPP_INCLUDE_POSTGRES_CONNECTION_HPP

#include <memory>
#include <unordered_set>
#include <string>
#include <utility>

#include <tao/postgres/transaction.hpp>
#include <tao/postgres/result.hpp>

// forward-declare libpq structures
struct pg_conn;
typedef struct pg_conn PGconn;

namespace tao
{
  namespace postgres
  {
    class connection_pool;
    class table_writer;

    namespace connection_impl
    {
      struct deleter final
      {
        void operator()( ::PGconn* p ) const;
      };
    }

    class connection final
      : public std::enable_shared_from_this< connection >
    {
    private:
      friend class connection_pool;
      friend class postgres::transaction;
      friend class table_writer;

      const std::unique_ptr< ::PGconn, connection_impl::deleter > pgconn_;
      postgres::transaction* current_transaction_;
      std::unordered_set< std::string > prepared_statements_;

      std::string error_message() const;
      void check_prepared_name( const std::string& name ) const;
      bool is_prepared( const char* name ) const;

      result execute_params( const char* statement, const int n_params, const char* const param_values[] );

    public:
      static std::shared_ptr< connection > create( const std::string& connect_info );

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

      bool is_open() const;

      void prepare( const std::string& name, const std::string& statement );
      void deallocate( const std::string& name );

      std::shared_ptr< postgres::transaction > direct();
      std::shared_ptr< postgres::transaction > transaction( const transaction::isolation_level isolation_level = transaction::isolation_level::DEFAULT );

      template< typename... Ts >
      result execute( Ts&&... ts )
      {
        return direct()->execute( std::forward< Ts >( ts )... );
      }

      // make sure you include libpq-fe.h before accessing the raw pointer
      ::PGconn* underlying_raw_ptr()
      {
        return pgconn_.get();
      }

      // make sure you include libpq-fe.h before accessing the raw pointer
      const ::PGconn* underlying_raw_ptr() const
      {
        return pgconn_.get();
      }
    };
  }
}

#endif // TAOCPP_INCLUDE_POSTGRES_CONNECTION_HPP
