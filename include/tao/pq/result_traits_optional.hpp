// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_RESULT_TRAITS_OPTIONAL_HPP
#define TAO_PQ_RESULT_TRAITS_OPTIONAL_HPP

#include <optional>

#include <tao/pq/result_traits.hpp>
#include <tao/pq/row.hpp>

namespace tao::pq
{
   template< typename T >
   struct result_traits< std::optional< T > >
   {
      static constexpr std::size_t size = result_traits_size< T >;

      [[nodiscard]] static auto null() noexcept -> std::optional< T >
      {
#if defined( __GNUC__ ) && !defined( __clang__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
         return {};
#if defined( __GNUC__ ) && !defined( __clang__ )
#pragma GCC diagnostic pop
#endif
      }

      [[nodiscard]] static auto from( const char* value ) -> std::optional< T >
      {
         return result_traits< T >::from( value );
      }

      [[nodiscard]] static auto from( const row& row ) -> std::optional< T >
      {
         for( std::size_t column = 0; column < row.columns(); ++column ) {
            if( !row.is_null( column ) ) {
               return result_traits< T >::from( row );
            }
         }
         return null();
      }
   };

}  // namespace tao::pq

#endif
