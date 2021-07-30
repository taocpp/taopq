// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TABLE_READER_HPP
#define TAO_PQ_TABLE_READER_HPP

#include <memory>
#include <string>

#include <libpq-fe.h>

namespace tao::pq
{
   namespace internal
   {
      class transaction;

   }  // namespace internal

   class table_reader
   {
   protected:
      std::shared_ptr< internal::transaction > m_previous;
      std::shared_ptr< internal::transaction > m_transaction;
      std::unique_ptr< char, decltype( &PQfreemem ) > m_buffer;

   public:
      table_reader( const std::shared_ptr< internal::transaction >& transaction, const std::string& statement );
      ~table_reader() = default;

      table_reader( const table_reader& ) = delete;
      table_reader( table_reader&& ) = delete;

      auto operator=( const table_reader& ) -> table_reader& = delete;
      auto operator=( table_reader&& ) -> table_reader& = delete;

      // this API will change, it is just a placeholder
      [[nodiscard]] auto fetch_next() -> bool;
      [[nodiscard]] auto get_data() -> const char*;
   };

}  // namespace tao::pq

#endif
