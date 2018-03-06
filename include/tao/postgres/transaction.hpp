// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_TRANSACTION_HPP
#define TAO_POSTGRES_TRANSACTION_HPP

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/postgres/parameter_traits.hpp>
#include <tao/postgres/result.hpp>
#include <tao/seq/make_integer_sequence.hpp>

namespace tao
{
   namespace postgres
   {
      class connection;
      class table_writer;

      class transaction
         : public std::enable_shared_from_this< transaction >
      {
      public:
         enum class isolation_level
         {
            DEFAULT,
            SERIALIZABLE,
            REPEATABLE_READ,
            READ_COMMITTED,
            READ_UNCOMMITTED
         };
         friend class table_writer;

      protected:
         std::shared_ptr< connection > connection_;

         explicit transaction( const std::shared_ptr< postgres::connection >& connection );
         virtual ~transaction() = 0;

         virtual bool v_is_direct() const = 0;

         virtual void v_commit() = 0;
         virtual void v_rollback() = 0;

         virtual void v_reset() noexcept = 0;

         transaction*& current_transaction() const;
         void check_current_transaction() const;

      private:
         result execute_params( const char* statement, const int n_params, const char* const param_values[] );

         template< std::size_t... Ns, typename... Ts >
         result execute_indexed_tuple( const char* statement, const seq::index_sequence< Ns... >&, const std::tuple< Ts... >& tuple )
         {
            const char* const param_values[] = { std::get< Ns >( tuple )... };
            return execute_params( statement, sizeof...( Ns ), param_values );
         }

         template< typename... Ts >
         result execute_tuple( const char* statement, const std::tuple< Ts... >& tuple )
         {
            return execute_indexed_tuple( statement, seq::index_sequence_for< Ts... >(), tuple );
         }

      public:
         transaction( const transaction& ) = delete;
         void operator=( const transaction& ) = delete;

         void commit();
         void rollback();

         std::shared_ptr< transaction > subtransaction();

         template< typename... As >
         result execute( const char* statement, As&&... as )
         {
            return execute_tuple( statement, std::tuple_cat( parameter_traits< typename std::decay< As >::type >( std::forward< As >( as ) )()... ) );
         }

         // short-cut for no-arguments invocations
         result execute( const char* statement )
         {
            return execute_params( statement, 0, nullptr );
         }

         template< typename... As >
         result execute( const std::string& statement, As&&... as )
         {
            return execute( statement.c_str(), std::forward< As >( as )... );
         }
      };

   }  // namespace postgres

}  // namespace tao

#endif
