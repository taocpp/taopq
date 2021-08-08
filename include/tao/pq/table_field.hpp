// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_TABLE_FIELD_HPP
#define TAO_PQ_TABLE_FIELD_HPP

#include <optional>
#include <string_view>
#include <type_traits>

#include <tao/pq/internal/dependent_false.hpp>
#include <tao/pq/internal/unreachable.hpp>
#include <tao/pq/null.hpp>
#include <tao/pq/result_traits.hpp>

namespace tao::pq
{
   class table_row;

   class table_field
   {
   private:
      friend class table_row;

      const table_row& m_row;
      const std::size_t m_column;

      table_field( const table_row& row, const std::size_t column ) noexcept
         : m_row( row ),
           m_column( column )
      {}

   public:
      [[nodiscard]] auto name() const -> std::string;

      [[nodiscard]] auto is_null() const -> bool;
      [[nodiscard]] auto get() const -> std::string_view;

      template< typename T >
      [[nodiscard]] auto as() const noexcept
         -> std::enable_if_t< result_traits_size< T > != 1, T >
      {
         static_assert( internal::dependent_false< T >, "tao::pq::result_traits<T>::size does not yield exactly one column for T, which is required for field access" );
         TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
      }

      template< typename T >
      [[nodiscard]] auto as() const
         -> std::enable_if_t< result_traits_size< T > == 1, T >;  // implemented in table_row.hpp

      template< typename T >
      [[nodiscard]] auto optional() const
      {
         return as< std::optional< T > >();
      }
   };

   [[nodiscard]] inline auto operator==( const table_field& f, null_t /*unused*/ )
   {
      return f.is_null();
   }

   [[nodiscard]] inline auto operator==( null_t /*unused*/, const table_field& f )
   {
      return f.is_null();
   }

   [[nodiscard]] inline auto operator!=( const table_field& f, null_t /*unused*/ )
   {
      return !f.is_null();
   }

   [[nodiscard]] inline auto operator!=( null_t /*unused*/, const table_field& f )
   {
      return !f.is_null();
   }

}  // namespace tao::pq

#endif
