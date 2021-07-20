// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TABLE_WRITER_HPP
#define TAO_PQ_TABLE_WRITER_HPP

#include <utility>

#include <tao/pq/internal/table_writer.hpp>
#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   template< template< typename... > class DefaultTraits >
   class basic_table_writer
      : public internal::table_writer
   {
   public:
      using internal::table_writer::table_writer;

      template< template< typename... > class Traits = DefaultTraits, typename... As >
      void insert( As&&... as )
      {
         return insert_traits( m_transaction->to_traits< Traits >( std::forward< As >( as ) )... );
      }
   };

   using table_writer = basic_table_writer< parameter_text_traits >;

}  // namespace tao::pq

#endif
