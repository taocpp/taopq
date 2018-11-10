// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_FIELD_HPP
#define TAO_POSTGRES_FIELD_HPP

#include <tao/postgres/null.hpp>
#include <tao/postgres/result_traits.hpp>

#include <optional>
#include <type_traits>

namespace tao
{
   namespace postgres
   {
      class row;

      class field
      {
      private:
         friend class row;

         const row& row_;
         const std::size_t column_;

         field( const row& row, const std::size_t column )
            : row_( row ),
              column_( column )
         {
         }

      public:
         [[nodiscard]] std::string name() const;

         [[nodiscard]] bool is_null() const;
         [[nodiscard]] const char* get() const;

         template< typename T >
         [[nodiscard]] std::enable_if_t< result_traits_size< T > != 1, T > as() const
         {
            static_assert( !std::is_same_v< T, T >, "tao::postgres::result_traits<T>::size does not yield exactly one column for T, which is required for field access" );
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

      [[nodiscard]] inline bool operator!=( const field& f, const null_t& )
      {
         return !f.is_null();
      }

   }  // namespace postgres

}  // namespace tao

#endif
