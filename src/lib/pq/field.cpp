// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#include <tao/pq/field.hpp>
#include <tao/pq/row.hpp>

namespace tao
{
   namespace pq
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

   }  // namespace pq

}  // namespace tao
