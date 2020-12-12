// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_FIELD_HPP
#define TAO_PQ_FIELD_HPP

#include <tao/pq/null.hpp>
#include <tao/pq/result_traits.hpp>

#include <optional>
#include <type_traits>

namespace tao::pq
{
   class row;

   class field
   {
   private:
      friend class row;

      const row& m_row;
      const std::size_t m_column;

      field( const row& row, const std::size_t column )
         : m_row( row ),
           m_column( column )
      {}

   public:
      [[nodiscard]] auto name() const -> std::string;

      [[nodiscard]] auto is_null() const -> bool;
      [[nodiscard]] auto get() const -> const char*;

      template< typename T >
      [[nodiscard]] auto as() const noexcept
         -> std::enable_if_t< result_traits_size< T > != 1, T >
      {
         static_assert( !std::is_same_v< T, T >, "tao::pq::result_traits<T>::size does not yield exactly one column for T, which is required for field access" );
#ifdef _WIN32
         __assume( false );
#else
         __builtin_unreachable();
#endif
      }

      template< typename T >
      [[nodiscard]] auto as() const
         -> std::enable_if_t< result_traits_size< T > == 1, T >;  // implemented in row.hpp

      template< typename T >
      [[nodiscard]] auto optional() const
      {
         return as< std::optional< T > >();
      }
   };

   [[nodiscard]] inline auto operator==( const field& f, const null_t& /*unused*/ )
   {
      return f.is_null();
   }

   [[nodiscard]] inline auto operator==( const null_t& /*unused*/, const field& f )
   {
      return f.is_null();
   }

   [[nodiscard]] inline auto operator!=( const field& f, const null_t& /*unused*/ )
   {
      return !f.is_null();
   }

   [[nodiscard]] inline auto operator!=( const null_t& /*unused*/, const field& f )
   {
      return !f.is_null();
   }

}  // namespace tao::pq

#endif
