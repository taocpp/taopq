// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
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
      {
      }

   public:
      [[nodiscard]] std::string name() const;

      [[nodiscard]] bool is_null() const;
      [[nodiscard]] const char* get() const;

      template< typename T >
      [[nodiscard]] std::enable_if_t< result_traits_size< T > != 1, T > as() const
      {
         static_assert( !std::is_same_v< T, T >, "tao::pq::result_traits<T>::size does not yield exactly one column for T, which is required for field access" );
         __builtin_unreachable();
      }

      template< typename T >
      [[nodiscard]] std::enable_if_t< result_traits_size< T > == 1, T > as() const;  // implemented in row.hpp

      template< typename T >
      [[nodiscard]] std::optional< T > optional() const
      {
         return as< std::optional< T > >();
      }
   };

   [[nodiscard]] inline bool operator==( const field& f, const null_t& )
   {
      return f.is_null();
   }

   [[nodiscard]] inline bool operator==( const null_t&, const field& f )
   {
      return f.is_null();
   }

   [[nodiscard]] inline bool operator!=( const field& f, const null_t& )
   {
      return !f.is_null();
   }

   [[nodiscard]] inline bool operator!=( const null_t&, const field& f )
   {
      return !f.is_null();
   }

}  // namespace tao::pq

#endif
