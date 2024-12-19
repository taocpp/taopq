// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_TRANSACTION_BASE_HPP
#define TAO_PQ_TRANSACTION_BASE_HPP

#include <chrono>
#include <cstddef>
#include <memory>
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

      void set_single_row_mode();
#if defined( LIBPQ_HAS_CHUNK_MODE )
      void set_chunk_mode( const int rows );
#endif

      [[nodiscard]] auto get_result( const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now() ) -> result;
      void consume_pipeline_sync( const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now() );
   };

}  // namespace tao::pq

#endif
