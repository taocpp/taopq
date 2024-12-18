// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_TRANSACTION_HPP
#define TAO_PQ_TRANSACTION_HPP

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <string_view>
#if !defined( __cpp_pack_indexing ) && ( __cplusplus >= 202302L )
#include <tuple>
#endif
#include <type_traits>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/internal/gen.hpp>
#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/parameter.hpp>
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/result.hpp>

namespace tao::pq
{
   class connection;
   class table_reader;
   class table_writer;

   class transaction_base
      : public std::enable_shared_from_this< transaction_base >
   {
   protected:
      std::shared_ptr< pq::connection > m_connection;

      friend class table_reader;
      friend class table_writer;

      explicit transaction_base( const std::shared_ptr< pq::connection >& connection ) noexcept;

   public:
      virtual ~transaction_base() = default;

      transaction_base( const transaction_base& ) = delete;
      transaction_base( transaction_base&& ) = delete;
      void operator=( const transaction_base& ) = delete;
      void operator=( transaction_base&& ) = delete;

   protected:
      [[nodiscard]] virtual auto v_is_direct() const noexcept -> bool = 0;

      [[nodiscard]] auto current_transaction() const noexcept -> transaction_base*&;
      void check_current_transaction() const;

      void send_params( const char* statement,
                        const int n_params,
                        const Oid types[],
                        const char* const values[],
                        const int lengths[],
                        const int formats[] );

#if defined( __cpp_pack_indexing ) && ( __cplusplus >= 202302L )

      template< std::size_t... Os, std::size_t... Is >
      void send_indexed( const char* statement,
                         std::index_sequence< Os... > /*unused*/,
                         std::index_sequence< Is... > /*unused*/,
                         const auto&... ts )
      {
         const Oid types[] = { static_cast< Oid >( ts...[ Os ].template type< Is >() )... };
         const char* const values[] = { ts...[ Os ].template value< Is >()... };
         const int lengths[] = { ts...[ Os ].template length< Is >()... };
         const int formats[] = { ts...[ Os ].template format< Is >()... };
         send_params( statement, sizeof...( Os ), types, values, lengths, formats );
      }

      template< typename... Ts >
      void send_traits( const char* statement, const Ts&... ts )
      {
         using gen = internal::gen< Ts::columns... >;
         transaction_base::send_indexed( statement, typename gen::outer_sequence(), typename gen::inner_sequence(), ts... );
      }

#else

      template< std::size_t... Os, std::size_t... Is, typename... Ts >
      void send_indexed( const char* statement,
                         std::index_sequence< Os... > /*unused*/,
                         std::index_sequence< Is... > /*unused*/,
                         const std::tuple< Ts... >& tuple )
      {
         const Oid types[] = { static_cast< Oid >( std::get< Os >( tuple ).template type< Is >() )... };
         const char* const values[] = { std::get< Os >( tuple ).template value< Is >()... };
         const int lengths[] = { std::get< Os >( tuple ).template length< Is >()... };
         const int formats[] = { std::get< Os >( tuple ).template format< Is >()... };
         send_params( statement, sizeof...( Os ), types, values, lengths, formats );
      }

      template< typename... Ts >
      void send_traits( const char* statement, const Ts&... ts )
      {
         using gen = internal::gen< Ts::columns... >;
         transaction_base::send_indexed( statement, typename gen::outer_sequence(), typename gen::inner_sequence(), std::tie( ts... ) );
      }

#endif

   public:
      [[nodiscard]] auto connection() const noexcept -> const std::shared_ptr< pq::connection >&
      {
         return m_connection;
      }

      void send( const internal::zsv statement )
      {
         send_params( statement, 0, nullptr, nullptr, nullptr, nullptr );
      }

      template< parameter_type_direct... As >
      void send( const internal::zsv statement, As&&... as )
      {
         send_traits( statement, parameter_traits< std::decay_t< As > >( std::forward< As >( as ) )... );
      }

      template< parameter_type... As >
         requires( parameter_type_dynamic< As > || ... )
      void send( const internal::zsv statement, As&&... as )
      {
         const parameter< internal::parameter_size< As... > > p( std::forward< As >( as )... );
         send_params( statement, p.m_size, p.m_types, p.m_values, p.m_lengths, p.m_formats );
      }

      template< parameter_type_dynamic A >
      void send( const internal::zsv statement, A&& p )
      {
         send_params( statement, p.m_size, p.m_types, p.m_values, p.m_lengths, p.m_formats );
      }

      [[nodiscard]] auto get_result( const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now() ) -> result;

      // TODO: move this to the pipeline_transaction class
      void consume_pipeline_sync( const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now() );
   };

   class transaction
      : public transaction_base
   {
   protected:
      using transaction_base::transaction_base;

      virtual void v_commit() = 0;
      virtual void v_rollback() = 0;

      virtual void v_reset() noexcept = 0;

   public:
      [[nodiscard]] auto subtransaction() -> std::shared_ptr< transaction >;

      void set_single_row_mode();
#if defined( LIBPQ_HAS_CHUNK_MODE )
      void set_chunk_mode( const int rows );
#endif

      template< parameter_type... As >
      auto execute( const internal::zsv statement, As&&... as )
      {
         const auto start = std::chrono::steady_clock::now();
         transaction::send( statement, std::forward< As >( as )... );
         return transaction::get_result( start );
      }

      void commit();
      void rollback();
   };

   namespace internal
   {
      class subtransaction_base
         : public transaction
      {
      private:
         const std::shared_ptr< pq::transaction_base > m_previous;

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
