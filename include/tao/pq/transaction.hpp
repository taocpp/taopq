// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TRANSACTION_HPP
#define TAO_PQ_TRANSACTION_HPP

#include <cstddef>
#include <cstdio>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/internal/gen.hpp>
#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/oid.hpp>
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   class connection;
   class table_reader;
   class table_writer;

   class transaction
      : public std::enable_shared_from_this< transaction >
   {
   protected:
      std::shared_ptr< pq::connection > m_connection;

      friend class table_reader;
      friend class table_writer;

      explicit transaction( const std::shared_ptr< pq::connection >& connection );

   public:
      virtual ~transaction() = default;

      transaction( const transaction& ) = delete;
      transaction( transaction&& ) = delete;
      void operator=( const transaction& ) = delete;
      void operator=( transaction&& ) = delete;

   protected:
      [[nodiscard]] virtual auto v_is_direct() const noexcept -> bool = 0;

      virtual void v_commit() = 0;
      virtual void v_rollback() = 0;

      virtual void v_reset() noexcept = 0;

      [[nodiscard]] auto current_transaction() const noexcept -> transaction*&;
      void check_current_transaction() const;

      [[nodiscard]] auto execute_params( const result::mode_t mode,
                                         const char* statement,
                                         const int n_params,
                                         const Oid types[],
                                         const char* const values[],
                                         const int lengths[],
                                         const int formats[] ) -> result;

      template< std::size_t... Os, std::size_t... Is, typename... Ts >
      [[nodiscard]] auto execute_indexed( const result::mode_t mode,
                                          const char* statement,
                                          std::index_sequence< Os... > /*unused*/,
                                          std::index_sequence< Is... > /*unused*/,
                                          const std::tuple< Ts... >& tuple )
      {
         const Oid types[] = { static_cast< Oid >( std::get< Os >( tuple ).template type< Is >() )... };
         const char* const values[] = { std::get< Os >( tuple ).template value< Is >()... };
         const int lengths[] = { std::get< Os >( tuple ).template length< Is >()... };
         const int formats[] = { std::get< Os >( tuple ).template format< Is >()... };
         return execute_params( mode, statement, sizeof...( Os ), types, values, lengths, formats );
      }

      template< typename... Ts >
      [[nodiscard]] auto execute_traits( const result::mode_t mode, const char* statement, const Ts&... ts )
      {
         using gen = internal::gen< Ts::columns... >;
         return transaction::execute_indexed( mode, statement, typename gen::outer_sequence(), typename gen::inner_sequence(), std::tie( ts... ) );
      }

      template< typename... As >
      auto execute_mode( const result::mode_t mode, const char* statement, As&&... as )
      {
         if constexpr( sizeof...( As ) == 0 ) {
            return execute_params( mode, statement, 0, nullptr, nullptr, nullptr, nullptr );
         }
         else {
            return execute_traits( mode, statement, parameter_traits< std::decay_t< As > >( std::forward< As >( as ) )... );
         }
      }

   public:
      [[nodiscard]] auto connection() const noexcept -> const std::shared_ptr< pq::connection >&
      {
         return m_connection;
      }

      [[nodiscard]] auto subtransaction() -> std::shared_ptr< transaction >;

      template< typename... As >
      auto execute( const internal::zsv statement, As&&... as )
      {
         return transaction::execute_mode( result::mode_t::expect_ok, statement, std::forward< As >( as )... );
      }

      void commit();
      void rollback();

      void listen( const std::string_view channel );
      void unlisten( const std::string_view channel );

      void notify( const std::string_view channel );
      void notify( const std::string_view channel, const std::string_view payload );
   };

   namespace internal
   {
      class subtransaction_base
         : public transaction
      {
      private:
         const std::shared_ptr< transaction > m_previous;

      protected:
         explicit subtransaction_base( const std::shared_ptr< pq::connection >& connection )
            : transaction( connection ),
              m_previous( current_transaction()->shared_from_this() )
         {
            current_transaction() = this;
         }

         ~subtransaction_base() override
         {
            if( m_connection ) {
               current_transaction() = m_previous.get();  // LCOV_EXCL_LINE
            }
         }

         [[nodiscard]] auto v_is_direct() const noexcept -> bool final
         {
            return false;
         }

         void v_reset() noexcept final
         {
            current_transaction() = m_previous.get();
            m_connection.reset();
         }

      public:
         subtransaction_base( const subtransaction_base& ) = delete;
         subtransaction_base( subtransaction_base&& ) = delete;
         void operator=( const subtransaction_base& ) = delete;
         void operator=( subtransaction_base&& ) = delete;
      };

      // blocker for table_reader and table_writer
      class transaction_guard final
         : public subtransaction_base
      {
      public:
         explicit transaction_guard( const std::shared_ptr< pq::connection >& connection )
            : subtransaction_base( connection )
         {}

      private:
         // LCOV_EXCL_START
         void v_commit() override {}
         void v_rollback() override {}
         // LCOV_EXCL_STOP
      };

   }  // namespace internal

}  // namespace tao::pq

#endif
