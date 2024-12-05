// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_TABLE_FIELD_HPP
#define TAO_PQ_TABLE_FIELD_HPP

#include <cstddef>
#include <optional>
#include <utility>

#include <tao/pq/null.hpp>
#include <tao/pq/result_traits.hpp>

namespace tao::pq
{
   class table_row;

   class table_field
   {
   private:
      friend class table_row;

      const table_row* m_row = nullptr;
      std::size_t m_column = 0;

      table_field() = default;

      table_field( const table_row& row, const std::size_t column ) noexcept
         : m_row( &row ),
           m_column( column )
      {}

   public:
      [[nodiscard]] auto index() const -> std::size_t;

      [[nodiscard]] auto is_null() const -> bool;
      [[nodiscard]] auto get() const -> const char*;

      template< result_type T >
         requires( result_traits_size< T > == 1 )
      [[nodiscard]] auto as() const -> T;  // implemented in table_row.hpp

      template< result_type T >
         requires( result_traits_size< T > == 1 )
      [[nodiscard]] auto optional() const
      {
         return as< std::optional< T > >();
      }

      friend void swap( table_field& lhs, table_field& rhs ) noexcept
      {
         std::swap( lhs.m_row, rhs.m_row );
         std::swap( lhs.m_column, rhs.m_column );
      }
   };

   [[nodiscard]] inline auto operator==( const table_field& f, null_t /*unused*/ )
   {
      return f.is_null();
   }

}  // namespace tao::pq

#endif
