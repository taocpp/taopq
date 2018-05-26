// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#include <tao/postgres/connection.hpp>

#include <cassert>
#include <cctype>
#include <cstring>
#include <stdexcept>

#include <libpq-fe.h>

namespace tao
{
   namespace postgres
   {
      namespace
      {
         bool is_identifier( const std::string& value )
         {
            if( value.empty() || std::isdigit( value[ 0 ] ) ) {
               return false;
            }
            for( auto c : value ) {
               if( !std::isalnum( c ) && c != '_' ) {
                  return false;
               }
            }
            return true;
         }

         class transaction_base
            : public transaction
         {
         protected:
            explicit transaction_base( const std::shared_ptr< postgres::connection >& connection )
               : transaction( connection )
            {
               if( current_transaction() ) {
                  throw std::logic_error( "transaction order error" );
               }
               current_transaction() = this;
            }

            ~transaction_base()
            {
               if( connection_ ) {
                  current_transaction() = nullptr;
               }
            }

            void v_reset() noexcept
            {
               current_transaction() = nullptr;
               connection_.reset();
            }
         };

         class autocommit_transaction final
            : public transaction_base
         {
         public:
            explicit autocommit_transaction( const std::shared_ptr< postgres::connection >& connection )
               : transaction_base( connection )
            {
            }

         private:
            bool v_is_direct() const
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
            const char* isolation_level_to_statement( const transaction::isolation_level il )
            {
               switch( il ) {
                  case transaction::isolation_level::DEFAULT:
                     return "START TRANSACTION";
                  case transaction::isolation_level::SERIALIZABLE:
                     return "START TRANSACTION ISOLATION LEVEL SERIALIZABLE";
                  case transaction::isolation_level::REPEATABLE_READ:
                     return "START TRANSACTION ISOLATION LEVEL REPEATABLE READ";
                  case transaction::isolation_level::READ_COMMITTED:
                     return "START TRANSACTION ISOLATION LEVEL READ COMMITTED";
                  case transaction::isolation_level::READ_UNCOMMITTED:
                     return "START TRANSACTION ISOLATION LEVEL READ UNCOMMITTED";
               }
               assert( !"code should be unreachable" );                   // LCOV_EXCL_LINE
               throw std::runtime_error( "code should be unreachable" );  // LCOV_EXCL_LINE
            }

         public:
            explicit top_level_transaction( const transaction::isolation_level il, const std::shared_ptr< postgres::connection >& connection )
               : transaction_base( connection )
            {
               execute( isolation_level_to_statement( il ) );
            }

            ~top_level_transaction()
            {
               if( connection_ && connection_->is_open() ) {
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
            bool v_is_direct() const
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

      namespace connection_impl
      {
         void deleter::operator()(::PGconn* p ) const
         {
            ::PQfinish( p );
         }
      }  // namespace connection_impl

      std::string connection::error_message() const
      {
         const char* message = ::PQerrorMessage( pgconn_.get() );
         assert( message );
         const std::size_t size = std::strlen( message );
         assert( size > 0 );
         assert( message[ size - 1 ] == '\n' );
         return std::string( message, size - 1 );
      }

      void connection::check_prepared_name( const std::string& name ) const
      {
         if( !postgres::is_identifier( name ) ) {
            throw std::invalid_argument( "invalid prepared statement name" );
         }
      }

      bool connection::is_prepared( const char* name ) const
      {
         return prepared_statements_.find( name ) != prepared_statements_.end();
      }

      result connection::execute_params( const char* statement, const int n_params, const char* const param_values[] )
      {
         if( n_params > 0 ) {
            assert( param_values );
         }
         if( is_prepared( statement ) ) {
            return result(::PQexecPrepared( pgconn_.get(), statement, n_params, param_values, nullptr, nullptr, 0 ) );
         }
         else {
            return result(::PQexecParams( pgconn_.get(), statement, n_params, nullptr, param_values, nullptr, nullptr, 0 ) );
         }
      }

      connection::connection( const connection::private_key&, const std::string& connect_info )
         : pgconn_(::PQconnectdb( connect_info.c_str() ), connection_impl::deleter() ),
           current_transaction_( nullptr )
      {
         if( !is_open() ) {
            throw std::runtime_error( "connection failed: " + error_message() );
         }
         const auto protocol_version = ::PQprotocolVersion( pgconn_.get() );
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
         return ::PQstatus( pgconn_.get() ) == CONNECTION_OK;
      }

      void connection::prepare( const std::string& name, const std::string& statement )
      {
         check_prepared_name( name );
         result(::PQprepare( pgconn_.get(), name.c_str(), statement.c_str(), 0, nullptr ) );
         prepared_statements_.insert( name );
      }

      void connection::deallocate( const std::string& name )
      {
         check_prepared_name( name );
         if( !is_prepared( name.c_str() ) ) {
            throw std::runtime_error( "prepared statement name not found: " + name );
         }
         execute( "DEALLOCATE " + name );
         prepared_statements_.erase( name );
      }

      std::shared_ptr< transaction > connection::direct()
      {
         return std::make_shared< autocommit_transaction >( shared_from_this() );
      }

      std::shared_ptr< transaction > connection::transaction( const transaction::isolation_level il )
      {
         return std::make_shared< top_level_transaction >( il, shared_from_this() );
      }

   }  // namespace postgres

}  // namespace tao
