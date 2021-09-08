// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/table_field.hpp>
#include <tao/pq/table_row.hpp>

namespace tao::pq
{
   auto table_field::is_null() const -> bool
   {
      return m_row.is_null( m_column );
   }

   auto table_field::get() const -> const char*
   {
      return m_row.get( m_column );
   }

}  // namespace tao::pq
