// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_PQ_TABLE_WRITER_HPP
#define TAO_PQ_TABLE_WRITER_HPP

#include <memory>
#include <string>

namespace tao
{
   namespace pq
   {
      class transaction;

      class table_writer
      {
      private:
         std::shared_ptr< transaction > transaction_;

      public:
         table_writer( const std::shared_ptr< transaction >& transaction, const std::string& statement );
         ~table_writer();

         void insert( const std::string& data );
         std::size_t finish();
      };

   }  // namespace pq

}  // namespace tao

#endif
