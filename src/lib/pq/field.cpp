// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/field.hpp>

#include <cstddef>
#include <string>

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
