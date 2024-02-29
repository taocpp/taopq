// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_FIELD_HPP
#define TAO_PQ_FIELD_HPP

#include <cstddef>
#include <optional>
#include <string>
#include <utility>

#include <tao/pq/null.hpp>
#include <tao/pq/result_traits.hpp>

namespace tao::pq
{
   class row;

   class field
   {
   private:
      friend class row;

      const row* m_row = nullptr;
      std::size_t m_column = 0;

      field() = default;

      field( const row& row, const std::size_t column ) noexcept
         : m_row( &row ),
           m_column( column )
      {}

   public:
      [[nodiscard]] auto name() const -> std::string;
      [[nodiscard]] auto index() const -> std::size_t;

      [[nodiscard]] auto is_null() const -> bool;
      [[nodiscard]] auto get() const -> const char*;

      template< typename T >
      [[nodiscard]] auto as() const -> T;  // implemented in row.hpp

      template< typename T >
      [[nodiscard]] auto optional() const
      {
         return as< std::optional< T > >();
      }

      friend void swap( field& lhs, field& rhs ) noexcept
      {
         std::swap( lhs.m_row, rhs.m_row );
         std::swap( lhs.m_column, rhs.m_column );
      }
   };

   [[nodiscard]] inline auto operator==( const field& f, null_t /*unused*/ )
   {
      return f.is_null();
   }

   [[nodiscard]] inline auto operator==( null_t /*unused*/, const field& f )
   {
      return f.is_null();
   }

   [[nodiscard]] inline auto operator!=( const field& f, null_t /*unused*/ )
   {
      return !f.is_null();
   }

   [[nodiscard]] inline auto operator!=( null_t /*unused*/, const field& f )
   {
      return !f.is_null();
   }

}  // namespace tao::pq

#endif
