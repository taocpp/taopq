// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
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
      [[nodiscard]] auto index() const noexcept -> std::size_t;

      [[nodiscard]] auto is_null() const -> bool;
      [[nodiscard]] auto get() const -> const char*;

      template< result_type T >
         requires( result_traits_size< T > == 1 )
      [[nodiscard]] auto as() const -> T;  // implemented in row.hpp

      template< result_type T >
         requires( result_traits_size< T > == 1 )
      [[nodiscard]] auto optional() const
      {
         return as< std::optional< T > >();
      }

      [[nodiscard]] auto operator==( null_t /*unused*/ ) const
      {
         return is_null();
      }

      friend void swap( field& lhs, field& rhs ) noexcept
      {
         std::swap( lhs.m_row, rhs.m_row );
         std::swap( lhs.m_column, rhs.m_column );
      }
   };

}  // namespace tao::pq

#endif
