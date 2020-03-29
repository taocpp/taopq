// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TABLE_WRITER_HPP
#define TAO_PQ_TABLE_WRITER_HPP

#include <memory>
#include <string>

namespace tao::pq
{
   class transaction;

   class table_writer
   {
   private:
      std::shared_ptr< transaction > m_transaction;

   public:
      table_writer( const std::shared_ptr< transaction >& transaction, const std::string& statement );
      ~table_writer();

      table_writer( const table_writer& ) = delete;
      table_writer( table_writer&& ) = delete;

      table_writer& operator=( const table_writer& ) = delete;
      table_writer& operator=( table_writer&& ) = delete;

      void insert( const std::string& data );
      auto finish() -> std::size_t;
   };

}  // namespace tao::pq

#endif
