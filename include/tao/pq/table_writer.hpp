// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_TABLE_WRITER_HPP
#define TAO_PQ_TABLE_WRITER_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#if !defined( __cpp_pack_indexing ) && ( __cplusplus >= 202302L )
#include <tuple>
#endif
#include <type_traits>
#include <utility>

#include <tao/pq/internal/gen.hpp>
#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   class table_writer final
   {
   protected:
      std::shared_ptr< transaction > m_previous;
      std::shared_ptr< transaction > m_transaction;

#if defined( __cpp_pack_indexing ) && ( __cplusplus >= 202302L )

      template< std::size_t... Os, std::size_t... Is >
      void insert_indexed( std::index_sequence< Os... > /*unused*/,
                           std::index_sequence< Is... > /*unused*/,
                           const auto&... ts )
      {
         std::string buffer;
         ( ( ts...[ Os ].template copy_to< Is >( buffer ), buffer += '\t' ), ... );
         *buffer.rbegin() = '\n';
         table_writer::insert_raw( buffer );
      }

      template< typename... Ts >
      void insert_traits( const Ts&... ts )
      {
         using gen = internal::gen< Ts::columns... >;
         table_writer::insert_indexed( typename gen::outer_sequence(), typename gen::inner_sequence(), ts... );
      }

#else

      template< std::size_t... Os, std::size_t... Is, typename... Ts >
      void insert_indexed( std::index_sequence< Os... > /*unused*/,
                           std::index_sequence< Is... > /*unused*/,
                           const std::tuple< Ts... >& tuple )
      {
         std::string buffer;
         ( ( std::get< Os >( tuple ).template copy_to< Is >( buffer ), buffer += '\t' ), ... );
         *buffer.rbegin() = '\n';
         table_writer::insert_raw( buffer );
      }

      template< typename... Ts >
      void insert_traits( const Ts&... ts )
      {
         using gen = internal::gen< Ts::columns... >;
         table_writer::insert_indexed( typename gen::outer_sequence(), typename gen::inner_sequence(), std::tie( ts... ) );
      }

#endif

      void check_result();

   public:
      template< typename... As >
      table_writer( const std::shared_ptr< transaction >& transaction, const internal::zsv statement, As&&... as )
         : m_previous( transaction ),
           m_transaction( std::make_shared< internal::transaction_guard >( transaction->connection() ) )
      {
         m_transaction->send( statement, std::forward< As >( as )... );
         check_result();
      }

      ~table_writer();

      table_writer( const table_writer& ) = delete;
      table_writer( table_writer&& ) = delete;
      void operator=( const table_writer& ) = delete;
      void operator=( table_writer&& ) = delete;

      void insert_raw( const std::string_view data );

      template< typename... As >
      void insert( As&&... as )
      {
         static_assert( sizeof...( As ) >= 1, "calling tao::pq::table_writer::insert() requires at least one argument" );
         return insert_traits( parameter_traits< std::decay_t< As > >( std::forward< As >( as ) )... );
      }

      auto commit() -> std::size_t;
   };

}  // namespace tao::pq

#endif
