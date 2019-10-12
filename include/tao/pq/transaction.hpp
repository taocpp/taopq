// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TRANSACTION_HPP
#define TAO_PQ_TRANSACTION_HPP

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/exclusive_scan.hpp>
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   namespace internal
   {
      template< typename, typename, typename >
      struct make;

      template< std::size_t... Is,
                std::size_t... Js,
                std::size_t... Ns >
      struct make< std::index_sequence< Is... >,
                   std::index_sequence< Js... >,
                   std::index_sequence< Ns... > >
      {
         template< std::size_t I >
         static constexpr std::size_t count = ( 0 + ... + ( ( Ns < I ) ? 1 : 0 ) );

         template< std::size_t J >
         static constexpr std::size_t select = ( 0 + ... + ( ( Js == J ) ? Ns : 0 ) );

         using outer = std::index_sequence< count< Is >... >;
         using inner = std::index_sequence< (Is - select< count< Is > >)... >;
      };

      template< std::size_t... Ns >
      using gen = make< std::make_index_sequence< ( 0 + ... + Ns ) >,
                        std::make_index_sequence< sizeof...( Ns ) >,
                        exclusive_scan_t< std::index_sequence< Ns... > > >;

   }  // namespace internal

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
         const int param_lengths[] = { std::get< Os >( tuple ).template size< Is >()... };
         const int param_formats[] = { std::get< Os >( tuple ).template format< Is >()... };
         return execute_params( statement, sizeof...( Os ), param_values, param_lengths, param_formats );
      }

      template< typename... Ts >
      [[nodiscard]] result execute_traits( const char* statement, const Ts&... ts )
      {
         using gen = internal::gen< Ts::columns... >;
         return execute_indexed( statement, typename gen::outer(), typename gen::inner(), std::tie( ts... ) );
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
