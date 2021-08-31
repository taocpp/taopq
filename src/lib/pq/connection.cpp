// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/connection.hpp>

#include <cassert>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>

#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/oid.hpp>

namespace tao::pq
{
   namespace
   {
      class transaction_base
         : public transaction
      {
      protected:
         explicit transaction_base( const std::shared_ptr< connection >& connection )
            : transaction( connection )
         {
            if( this->current_transaction() != nullptr ) {
               throw std::logic_error( "transaction order error" );
            }
            this->current_transaction() = this;
         }

         ~transaction_base() override
         {
            if( this->m_connection ) {
               this->current_transaction() = nullptr;
            }
         }

         void v_reset() noexcept override
         {
            this->current_transaction() = nullptr;
            this->m_connection.reset();
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
         explicit autocommit_transaction( const std::shared_ptr< connection >& connection )
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
         [[nodiscard]] static auto isolation_level_extension( const isolation_level il ) -> const char*
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

         [[nodiscard]] static auto access_mode_extension( const access_mode am ) -> const char*
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

      public:
         explicit top_level_transaction( const std::shared_ptr< connection >& connection, const isolation_level il, const access_mode am )
            : transaction_base( connection )
         {
            this->execute( std::string( "START TRANSACTION" ) + isolation_level_extension( il ) + access_mode_extension( am ) );
         }

         ~top_level_transaction() override
         {
            if( this->m_connection && this->m_connection->is_open() ) {
               try {
                  this->rollback();
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
            this->execute( "COMMIT TRANSACTION" );
         }

         void v_rollback() override
         {
            this->execute( "ROLLBACK TRANSACTION" );
         }
      };

      [[nodiscard]] constexpr auto is_identifier( const std::string_view value ) noexcept -> bool
      {
         return !value.empty() && ( value.find_first_not_of( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ) == std::string_view::npos ) && ( std::isdigit( value[ 0 ] ) == 0 );
      }

   }  // namespace

   auto connection::error_message() const -> std::string
   {
      const char* message = PQerrorMessage( m_pgconn.get() );
      assert( message );
      const std::size_t size = std::strlen( message );
      assert( size > 0 );
      assert( message[ size - 1 ] == '\n' );
      return std::string( message, size - 1 );
   }

   auto connection::escape_identifier( const std::string_view identifier ) const -> std::string
   {
      const std::unique_ptr< char, decltype( &PQfreemem ) > buffer( PQescapeIdentifier( m_pgconn.get(), identifier.data(), identifier.size() ), &PQfreemem );
      if( !buffer ) {
         throw std::runtime_error( "PQescapeIdentifier failed: " + error_message() );  // LCOV_EXCL_LINE
      }
      return buffer.get();
   }

   void connection::check_prepared_name( const std::string_view name )
   {
      if( !is_identifier( name ) ) {
         throw std::invalid_argument( "invalid prepared statement name" );
      }
   }

   auto connection::is_prepared( const std::string_view name ) const noexcept -> bool
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

   connection::connection( const std::string& connection_info )
      : m_pgconn( PQconnectdb( connection_info.c_str() ), internal::deleter() ),
        m_current_transaction( nullptr )
   {
      if( !is_open() ) {
         throw std::runtime_error( "connection failed: " + error_message() );
      }
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
      if( !is_prepared( name ) ) {
         throw std::runtime_error( "prepared statement name not found: " + name );
      }
      (void)execute_params( ( "DEALLOCATE " + escape_identifier( name ) ).c_str(), 0, nullptr, nullptr, nullptr, nullptr );
      m_prepared_statements.erase( name );
   }

   auto connection::create( const std::string& connection_info ) -> std::shared_ptr< connection >
   {
      return std::make_shared< connection >( connection_info );
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

}  // namespace tao::pq
