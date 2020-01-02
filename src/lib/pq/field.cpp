// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/field.hpp>
#include <tao/pq/row.hpp>

namespace tao::pq
{
   std::string field::name() const
   {
      return m_row.name( m_column );
   }

   bool field::is_null() const
   {
      return m_row.is_null( m_column );
   }

   const char* field::get() const
   {
      return m_row.get( m_column );
   }

}  // namespace tao::pq
