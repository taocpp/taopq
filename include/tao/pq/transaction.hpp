// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TRANSACTION_HPP
#define TAO_PQ_TRANSACTION_HPP

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/gen.hpp>
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
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
      std::shared_ptr< connection > m_connection;

      explicit transaction( const std::shared_ptr< pq::connection >& connection );
      virtual ~transaction() = 0;

      [[nodiscard]] virtual bool v_is_direct() const noexcept = 0;

      virtual void v_commit() = 0;
      virtual void v_rollback() = 0;

      virtual void v_reset() noexcept = 0;

      [[nodiscard]] transaction*& current_transaction() const noexcept;
      void check_current_transaction() const;

   private:
      [[nodiscard]] result execute_params( const char* statement,
                                           const int n_params,
                                           const Oid types[],
                                           const char* const values[],
                                           const int lengths[],
                                           const int formats[] );

      template< std::size_t... Os, std::size_t... Is, typename... Ts >
      [[nodiscard]] result execute_indexed( const char* statement,
                                            std::index_sequence< Os... > /*unused*/,
                                            std::index_sequence< Is... > /*unused*/,
                                            const std::tuple< Ts... >& tuple )
      {
         const Oid types[] = { std::get< Os >( tuple ).template type< Is >()... };
         const char* const values[] = { std::get< Os >( tuple ).template value< Is >()... };
         const int lengths[] = { std::get< Os >( tuple ).template length< Is >()... };
         const int formats[] = { std::get< Os >( tuple ).template format< Is >()... };
         return execute_params( statement, sizeof...( Os ), types, values, lengths, formats );
      }

      template< typename... Ts >
      [[nodiscard]] result execute_traits( const char* statement, const Ts&... ts )
      {
         using gen = internal::gen< Ts::columns... >;
         return execute_indexed( statement, typename gen::outer_sequence(), typename gen::inner_sequence(), std::tie( ts... ) );
      }

   public:
      transaction( const transaction& ) = delete;
      transaction( transaction&& ) = delete;
      void operator=( const transaction& ) = delete;
      void operator=( transaction&& ) = delete;

      void commit();
      void rollback();

      [[nodiscard]] std::shared_ptr< transaction > subtransaction();

      template< template< typename... > class Traits = parameter_traits, typename... As >
      result execute( const char* statement, As&&... as )
      {
         return execute_traits( statement, Traits< std::decay_t< As > >( std::forward< As >( as ) )... );
      }

      // short-cut for no-arguments invocations
      template< template< typename... > class Traits = parameter_traits >
      result execute( const char* statement )
      {
         return execute_params( statement, 0, nullptr, nullptr, nullptr, nullptr );
      }

      template< template< typename... > class Traits = parameter_traits, typename... As >
      result execute( const std::string& statement, As&&... as )
      {
         return execute< Traits >( statement.c_str(), std::forward< As >( as )... );
      }
   };

}  // namespace tao::pq

#endif
