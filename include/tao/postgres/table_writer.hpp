// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_TABLE_WRITER_HPP
#define TAO_POSTGRES_TABLE_WRITER_HPP

#include <memory>
#include <string>

namespace tao
{
   namespace postgres
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

   }  // namespace postgres

}  // namespace tao

#endif
