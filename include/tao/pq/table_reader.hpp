// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TABLE_READER_HPP
#define TAO_PQ_TABLE_READER_HPP

#include <memory>
#include <string>
#include <string_view>
#include <vector>

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
      std::vector< std::string_view > m_fields;

   public:
      table_reader( const std::shared_ptr< internal::transaction >& transaction, const std::string& statement );
      ~table_reader() = default;

      table_reader( const table_reader& ) = delete;
      table_reader( table_reader&& ) = delete;

      auto operator=( const table_reader& ) -> table_reader& = delete;
      auto operator=( table_reader&& ) -> table_reader& = delete;

      // note: the following API is experimental and subject to change

      [[nodiscard]] auto get_raw_data() -> std::string_view;
      [[nodiscard]] auto parse_data() noexcept -> bool;
      [[nodiscard]] auto get_row() -> bool;

      // note: these string views are guaranteed to be zero-terminated
      [[nodiscard]] auto fields() const noexcept -> const std::vector< std::string_view >&
      {
         return m_fields;
      }

      // TODO: add conversions using the result traits
   };

}  // namespace tao::pq

#endif
