// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TABLE_WRITER_HPP
#define TAO_PQ_TABLE_WRITER_HPP

#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include <tao/pq/internal/gen.hpp>
#include <tao/pq/internal/to_traits.hpp>
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   class table_writer final
   {
   protected:
      std::shared_ptr< transaction > m_previous;
      std::shared_ptr< transaction > m_transaction;

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

   public:
      template< typename... As >
      table_writer( const std::shared_ptr< transaction >& transaction, const std::string& statement, As&&... as )
         : m_previous( transaction ),
           m_transaction( std::make_shared< internal::transaction_guard >( transaction->m_connection ) )
      {
         m_transaction->execute_mode( result::mode_t::expect_copy_in, statement.c_str(), std::forward< As >( as )... );
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
         static_assert( sizeof...( As ) >= 1, "tao::pq::table_writer::insert() needs at least one argument" );
         return insert_traits( internal::to_traits( std::forward< As >( as ) )... );
      }

      auto commit() -> std::size_t;
   };

}  // namespace tao::pq

#endif
