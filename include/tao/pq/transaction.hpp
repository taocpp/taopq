// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
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

      virtual bool v_is_direct() const = 0;

      virtual void v_commit() = 0;
      virtual void v_rollback() = 0;

      virtual void v_reset() noexcept = 0;

      [[nodiscard]] transaction*& current_transaction() const noexcept;
      void check_current_transaction() const;

   private:
      [[nodiscard]] result execute_params( const char* statement,
                                           const int n_params,
                                           const char* const param_values[],
                                           const int param_lengths[],
                                           const int param_formats[] );

      template< std::size_t... Os, std::size_t... Is, typename... Ts >
      [[nodiscard]] result execute_indexed( const char* statement,
                                            std::index_sequence< Os... > /*unused*/,
                                            std::index_sequence< Is... > /*unused*/,
                                            const std::tuple< Ts... >& tuple )
      {
         const char* const param_values[] = { std::get< Os >( tuple ).template c_str< Is >()... };
         constexpr const int param_lengths[] = { std::decay_t< std::tuple_element_t< Os, std::tuple< Ts... > > >::template size< Is >()... };
         constexpr const int param_formats[] = { std::decay_t< std::tuple_element_t< Os, std::tuple< Ts... > > >::template format< Is >()... };
         return execute_params( statement, sizeof...( Os ), param_values, param_lengths, param_formats );
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
         return execute_params( statement, 0, nullptr, nullptr, nullptr );
      }

      template< template< typename... > class Traits = parameter_traits, typename... As >
      result execute( const std::string& statement, As&&... as )
      {
         return execute< Traits >( statement.c_str(), std::forward< As >( as )... );
      }
   };

}  // namespace tao::pq

#endif
