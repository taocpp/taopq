// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TABLE_WRITER_HPP
#define TAO_PQ_TABLE_WRITER_HPP

#include <memory>
#include <string>

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

   public:
      table_writer( const std::shared_ptr< internal::transaction >& transaction, const std::string& statement );
      ~table_writer();

      table_writer( const table_writer& ) = delete;
      table_writer( table_writer&& ) = delete;

      // clang-format off
      auto operator=( const table_writer& ) -> table_writer& = delete;
      auto operator=( table_writer&& ) -> table_writer& = delete;
      // clang-format on

      void insert( const std::string& data );
      auto finish() -> std::size_t;
   };

}  // namespace tao::pq

#endif
