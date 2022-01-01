// Copyright (c) 2021-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/table_field.hpp>
#include <tao/pq/table_row.hpp>

namespace tao::pq
{
   auto table_field::index() const -> std::size_t
   {
      return m_column - m_row->m_offset;
   }

   auto table_field::is_null() const -> bool
   {
      return m_row->is_null( m_column );
   }

   auto table_field::get() const -> const char*
   {
      return m_row->get( m_column );
   }

}  // namespace tao::pq
