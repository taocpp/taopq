// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#include <tao/postgres/field.hpp>
#include <tao/postgres/row.hpp>

namespace tao
{
   namespace postgres
   {
      std::string field::name() const
      {
         return row_.name( column_ );
      }

      bool field::is_null() const
      {
         return row_.is_null( column_ );
      }

      const char* field::get() const
      {
         return row_.get( column_ );
      }

   }  // namespace postgres

}  // namespace tao
