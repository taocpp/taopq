// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/field.hpp>
#include <tao/pq/row.hpp>

namespace tao::pq
{
   auto field::name() const -> std::string
   {
      return m_row->name( m_column );
   }

   auto field::index() const -> std::size_t
   {
      return m_column - m_row->m_offset;
   }

   auto field::is_null() const -> bool
   {
      return m_row->is_null( m_column );
   }

   auto field::get() const -> const char*
   {
      return m_row->get( m_column );
   }

}  // namespace tao::pq
