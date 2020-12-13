// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <cassert>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <string_view>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/unreachable.hpp>

namespace tao::pq
{
   namespace
   {
      [[nodiscard]] constexpr auto is_identifier( const std::string_view value ) noexcept -> bool
      {
         return !value.empty() && ( value.find_first_not_of( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ) == std::string_view::npos ) && ( std::isdigit( value[ 0 ] ) == 0 );
      }

      class transaction_base
         : public transaction
      {
      protected:
         explicit transaction_base( const std::shared_ptr< pq::connection >& connection )
            : transaction( connection )
         {
            if( current_transaction() != nullptr ) {
               throw std::logic_error( "transaction order error" );
            }
            current_transaction() = this;
         }

         ~transaction_base() override
         {
            if( m_connection ) {
               current_transaction() = nullptr;
            }
         }

         void v_reset() noexcept override
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

      class top_level_transaction final
         : public transaction_base
      {
      private:
         [[nodiscard]] static auto isolation_level_to_statement( const transaction::isolation_level il ) -> const char*
         {
            switch( il ) {
               case transaction::isolation_level::default_isolation_level:
                  return "START TRANSACTION";
               case transaction::isolation_level::serializable:
                  return "START TRANSACTION ISOLATION LEVEL SERIALIZABLE";
               case transaction::isolation_level::repeatable_read:
                  return "START TRANSACTION ISOLATION LEVEL REPEATABLE READ";
               case transaction::isolation_level::read_committed:
                  return "START TRANSACTION ISOLATION LEVEL READ COMMITTED";
               case transaction::isolation_level::read_uncommitted:
                  return "START TRANSACTION ISOLATION LEVEL READ UNCOMMITTED";
            }
            TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
         }

      public:
         explicit top_level_transaction( const transaction::isolation_level il, const std::shared_ptr< pq::connection >& connection )
            : transaction_base( connection )
         {
            execute( isolation_level_to_statement( il ) );
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

   }  // namespace

   namespace internal
   {
      void deleter::operator()( PGconn* p ) const noexcept
      {
         PQfinish( p );
      }

   }  // namespace internal

   auto connection::error_message() const -> std::string
   {
      const char* message = PQerrorMessage( m_pgconn.get() );
      assert( message );
      const std::size_t size = std::strlen( message );
      assert( size > 0 );
      assert( message[ size - 1 ] == '\n' );
      return std::string( message, size - 1 );
   }

   void connection::check_prepared_name( const std::string& name )
   {
      if( !pq::is_identifier( name ) ) {
         throw std::invalid_argument( "invalid prepared statement name" );
      }
   }

   auto connection::is_prepared( const char* name ) const noexcept -> bool
   {
      return m_prepared_statements.find( name ) != m_prepared_statements.end();
   }

   auto connection::execute_params( const char* statement,
                                    const int n_params,
                                    const Oid types[],
                                    const char* const values[],
                                    const int lengths[],
                                    const int formats[] ) -> result
   {
      if( is_prepared( statement ) ) {
         return result( PQexecPrepared( m_pgconn.get(), statement, n_params, values, lengths, formats, 0 ) );
      }
      return result( PQexecParams( m_pgconn.get(), statement, n_params, types, values, lengths, formats, 0 ) );
   }

   connection::connection( const connection::private_key& /*unused*/, const std::string& connection_info )
      : m_pgconn( PQconnectdb( connection_info.c_str() ), internal::deleter() ),
        m_current_transaction( nullptr )
   {
      if( !is_open() ) {
         throw std::runtime_error( "connection failed: " + error_message() );
      }
      const auto protocol_version = PQprotocolVersion( m_pgconn.get() );
      if( protocol_version < 3 ) {
         throw std::runtime_error( "protocol version 3 required" );  // LCOV_EXCL_LINE
      }
      // TODO: check server version
   }

   auto connection::create( const std::string& connection_info ) -> std::shared_ptr< connection >
   {
      return std::make_shared< connection >( private_key(), connection_info );
   }

   auto connection::is_open() const noexcept -> bool
   {
      return PQstatus( m_pgconn.get() ) == CONNECTION_OK;
   }

   void connection::prepare( const std::string& name, const std::string& statement )
   {
      check_prepared_name( name );
      result( PQprepare( m_pgconn.get(), name.c_str(), statement.c_str(), 0, nullptr ) );  // NOLINT(bugprone-unused-raii)
      m_prepared_statements.insert( name );
   }

   void connection::deallocate( const std::string& name )
   {
      check_prepared_name( name );
      if( !is_prepared( name.c_str() ) ) {
         throw std::runtime_error( "prepared statement name not found: " + name );
      }
      execute( "DEALLOCATE " + name );
      m_prepared_statements.erase( name );
   }

   auto connection::direct() -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< autocommit_transaction >( shared_from_this() );
   }

   auto connection::transaction( const transaction::isolation_level il ) -> std::shared_ptr< pq::transaction >
   {
      return std::make_shared< top_level_transaction >( il, shared_from_this() );
   }

}  // namespace tao::pq
