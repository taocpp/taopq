// The Art of C++ / taopq
// Copyright (c) 2016-2019 Daniel Frey

#include <tao/pq/field.hpp>
#include <tao/pq/row.hpp>

namespace tao::pq
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

}  // namespace tao::pq
