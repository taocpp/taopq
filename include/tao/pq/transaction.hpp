// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_PQ_TRANSACTION_HPP
#define TAO_PQ_TRANSACTION_HPP

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/result.hpp>

namespace tao
{
   namespace pq
   {
      class connection;
      class table_writer;

      class transaction
         : public std::enable_shared_from_this< transaction >
      {
      public:
         enum class isolation_level
         {
            default_isolation_level,
            serializable,
            repeatable_read,
            read_committed,
            read_uncommitted
         };
         friend class table_writer;

      protected:
         std::shared_ptr< connection > connection_;

         explicit transaction( const std::shared_ptr< pq::connection >& connection );
         virtual ~transaction() = 0;

         virtual bool v_is_direct() const = 0;

         virtual void v_commit() = 0;
         virtual void v_rollback() = 0;

         virtual void v_reset() noexcept = 0;

         [[nodiscard]] transaction*& current_transaction() const noexcept;
         void check_current_transaction() const;

      private:
         [[nodiscard]] result execute_params( const std::string& statement, const int n_params, const char* const param_values[] );

         template< std::size_t... Ns, typename... Ts >
         [[nodiscard]] result execute_indexed_tuple( const std::string& statement, std::index_sequence< Ns... >, const std::tuple< Ts... >& tuple )
         {
            const char* const param_values[] = { std::get< Ns >( tuple )... };
            return execute_params( statement, sizeof...( Ns ), param_values );
         }

         template< typename... Ts >
         [[nodiscard]] result execute_tuple( const std::string& statement, const std::tuple< Ts... >& tuple )
         {
            return execute_indexed_tuple( statement, std::index_sequence_for< Ts... >(), tuple );
         }

      public:
         transaction( const transaction& ) = delete;
         void operator=( const transaction& ) = delete;

         void commit();
         void rollback();

         [[nodiscard]] std::shared_ptr< transaction > subtransaction();

         template< typename... As >
         result execute( const std::string& statement, As&&... as )
         {
            return execute_tuple( statement, std::tuple_cat( parameter_traits< std::decay_t< As > >( std::forward< As >( as ) )()... ) );
         }

         // short-cut for no-arguments invocations
         result execute( const std::string& statement )
         {
            return execute_params( statement, 0, nullptr );
         }
      };

   }  // namespace pq

}  // namespace tao

#endif
