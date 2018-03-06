// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_FIELD_HPP
#define TAO_POSTGRES_FIELD_HPP

#include <tao/optional/optional.hpp>
#include <tao/postgres/null.hpp>
#include <tao/postgres/result_traits.hpp>
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
         std::string name() const;

         bool is_null() const;
         const char* get() const;

         template< typename T >
         typename std::enable_if< result_traits_size< T >::value != 1, T >::type as() const
         {
            static_assert( !std::is_same< T, T >::value, "tao::postgres::result_traits<T>::size does not yield exactly one column for T, which is required for field access" );
            __builtin_unreachable();
         }

         template< typename T >
         typename std::enable_if< result_traits_size< T >::value == 1, T >::type as() const;  // implemented in row.hpp

         template< typename T >
         tao::optional< T > optional() const
         {
            return as< tao::optional< T > >();
         }
      };

      inline bool operator==( const field& f, const null_t& )
      {
         return f.is_null();
      }

      inline bool operator!=( const field& f, const null_t& )
      {
         return !f.is_null();
      }

   }  // namespace postgres

}  // namespace tao

#endif
