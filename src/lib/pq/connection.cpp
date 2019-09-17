// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <cassert>
#include <cctype>
#include <cstring>
#include <stdexcept>

#include <libpq-fe.h>

#include <tao/pq/connection.hpp>

namespace tao::pq
{
   namespace
   {
      [[nodiscard]] bool is_identifier( const std::string& value ) noexcept
      {
         if( value.empty() || std::isdigit( value[ 0 ] ) ) {
            return false;
         }
         for( auto c : value ) {
            if( !std::isalnum( c ) && ( c != '_' ) ) {
               return false;
            }
         }
         return true;
      }

      class transaction_base
         : public transaction
      {
      protected:
         explicit transaction_base( const std::shared_ptr< pq::connection >& connection )
            : transaction( connection )
         {
            if( current_transaction() ) {
               throw std::logic_error( "transaction order error" );
            }
            current_transaction() = this;
         }

         ~transaction_base()
         {
            if( m_connection ) {
               current_transaction() = nullptr;
            }
         }

         void v_reset() noexcept
         {
            current_transaction() = nullptr;
            m_connection.reset();
         }
      };

      class autocommit_transaction final
         : public transaction_base
      {
      public:
         explicit autocommit_transaction( const std::shared_ptr< pq::connection >& connection )
            : transaction_base( connection )
         {
         }

      private:
         [[nodiscard]] bool v_is_direct() const
         {
            return true;
         }

         void v_commit()
         {
         }

         void v_rollback()
         {
         }
      };

      class top_level_transaction final
         : public transaction_base
      {
      private:
         [[nodiscard]] const char* isolation_level_to_statement( const transaction::isolation_level il )
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
            assert( !"code should be unreachable" );                   // LCOV_EXCL_LINE
            throw std::runtime_error( "code should be unreachable" );  // LCOV_EXCL_LINE
         }

      public:
         explicit top_level_transaction( const transaction::isolation_level il, const std::shared_ptr< pq::connection >& connection )
            : transaction_base( connection )
         {
            execute( isolation_level_to_statement( il ) );
         }

         ~top_level_transaction()
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

      private:
         [[nodiscard]] bool v_is_direct() const
         {
            return false;
         }

         void v_commit()
         {
            execute( "COMMIT TRANSACTION" );
         }

         void v_rollback()
         {
            execute( "ROLLBACK TRANSACTION" );
         }
      };

   }  // namespace

   namespace internal
   {
      void deleter::operator()( ::PGconn* p ) const noexcept
      {
         ::PQfinish( p );
      }

   }  // namespace internal

   std::string connection::error_message() const
   {
      const char* message = ::PQerrorMessage( m_pgconn.get() );
      assert( message );
      const std::size_t size = std::strlen( message );
      assert( size > 0 );
      assert( message[ size - 1 ] == '\n' );
      return std::string( message, size - 1 );
   }

   void connection::check_prepared_name( const std::string& name ) const
   {
      if( !pq::is_identifier( name ) ) {
         throw std::invalid_argument( "invalid prepared statement name" );
      }
   }

   bool connection::is_prepared( const char* name ) const noexcept
   {
      return m_prepared_statements.find( name ) != m_prepared_statements.end();
   }

   result connection::execute_params( const char* statement,
                                      const int n_params,
                                      const char* const param_values[],
                                      const int param_lengths[],
                                      const int param_formats[] )
   {
      if( is_prepared( statement ) ) {
         return result( ::PQexecPrepared( m_pgconn.get(), statement, n_params, param_values, param_lengths, param_formats, 0 ) );
      }
      else {
         return result( ::PQexecParams( m_pgconn.get(), statement, n_params, nullptr, param_values, param_lengths, param_formats, 0 ) );
      }
   }

   connection::connection( const connection::private_key&, const std::string& connect_info )
      : m_pgconn( ::PQconnectdb( connect_info.c_str() ), internal::deleter() ),
        m_current_transaction( nullptr )
   {
      if( !is_open() ) {
         throw std::runtime_error( "connection failed: " + error_message() );
      }
      const auto protocol_version = ::PQprotocolVersion( m_pgconn.get() );
      if( protocol_version < 3 ) {
         throw std::runtime_error( "protocol version 3 required" );  // LCOV_EXCL_LINE
      }
      // TODO: check server version
   }

   std::shared_ptr< connection > connection::create( const std::string& connect_info )
   {
      return std::make_shared< connection >( private_key(), connect_info );
   }

   bool connection::is_open() const noexcept
   {
      return ::PQstatus( m_pgconn.get() ) == CONNECTION_OK;
   }

   void connection::prepare( const std::string& name, const std::string& statement )
   {
      check_prepared_name( name );
      result( ::PQprepare( m_pgconn.get(), name.c_str(), statement.c_str(), 0, nullptr ) );
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

   std::shared_ptr< transaction > connection::direct()
   {
      return std::make_shared< autocommit_transaction >( shared_from_this() );
   }

   std::shared_ptr< transaction > connection::transaction( const transaction::isolation_level il )
   {
      return std::make_shared< top_level_transaction >( il, shared_from_this() );
   }

}  // namespace tao::pq
