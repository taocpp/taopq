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
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   namespace internal
   {
      class transaction;

   }  // namespace internal

   class table_writer
   {
   private:
      std::shared_ptr< internal::transaction > m_transaction;

      void insert_values( const std::string_view values[], const bool escape[], const std::size_t n_values );

      template< std::size_t... Os, std::size_t... Is, typename... Ts >
      void insert_indexed( std::index_sequence< Os... > /*unused*/,
                           std::index_sequence< Is... > /*unused*/,
                           const std::tuple< Ts... >& tuple )
      {
         const std::string_view values[] = { std::get< Os >( tuple ).template string_view< Is >()... };
         const bool escape[] = { std::get< Os >( tuple ).template escape< Is >()... };
         insert_values( values, escape, sizeof...( Os ) );
      }

      template< typename... Ts >
      void insert_traits( const Ts&... ts )
      {
         using gen = internal::gen< Ts::columns... >;
         insert_indexed( typename gen::outer_sequence(), typename gen::inner_sequence(), std::tie( ts... ) );
      }

   public:
      table_writer( const std::shared_ptr< internal::transaction >& transaction, const std::string& statement );
      ~table_writer();

      table_writer( const table_writer& ) = delete;
      table_writer( table_writer&& ) = delete;

      // clang-format off
      auto operator=( const table_writer& ) -> table_writer& = delete;
      auto operator=( table_writer&& ) -> table_writer& = delete;
      // clang-format on

      void insert_raw( const std::string_view data );

      template< template< typename... > class Traits = parameter_text_traits, typename... As >
      void insert( As&&... as )
      {
         return insert_traits( m_transaction->to_traits< Traits >( std::forward< As >( as ) )... );
      }

      auto finish() -> std::size_t;
   };

}  // namespace tao::pq

#endif
