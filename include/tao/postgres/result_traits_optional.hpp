// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_RESULT_TRAITS_OPTIONAL_HPP
#define TAO_POSTGRES_RESULT_TRAITS_OPTIONAL_HPP

#include <tao/optional/optional.hpp>

#include <tao/postgres/result_traits.hpp>
#include <tao/postgres/row.hpp>

namespace tao
{
   namespace postgres
   {
      template< typename T >
      struct result_traits< optional< T > >
      {
         static constexpr std::size_t size = result_traits_size< T >::value;

         static optional< T > null()
         {
            return optional< T >();
         }

         static optional< T > from( const char* value )
         {
            return optional< T >( result_traits< T >::from( value ) );
         }

         static optional< T > from( const row& row )
         {
            for( std::size_t column = 0; column < row.columns(); ++column ) {
               if( !row.is_null( column ) ) {
                  return optional< T >( result_traits< T >::from( row ) );
               }
            }
            return null();
         }
      };

   }  // namespace postgres

}  // namespace tao

#endif
