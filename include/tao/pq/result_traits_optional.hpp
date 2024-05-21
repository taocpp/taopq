// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_TRAITS_OPTIONAL_HPP
#define TAO_PQ_RESULT_TRAITS_OPTIONAL_HPP

#include <optional>

#include <tao/pq/result_traits.hpp>
#include <tao/pq/row.hpp>

template< typename T >
struct tao::pq::result_traits< std::optional< T > >
{
   static constexpr std::size_t size = result_traits_size< T >;

   [[nodiscard]] static auto null() noexcept -> std::optional< T >
   {
      return std::nullopt;
   }

   [[nodiscard]] static auto from( const char* value ) -> std::optional< T >
   {
      return result_traits< T >::from( value );
   }

   template< typename Row >
   [[nodiscard]] static auto from( const Row& row ) -> std::optional< T >
   {
      for( std::size_t column = 0; column < row.columns(); ++column ) {
         if( !row.is_null( column ) ) {
            return result_traits< T >::from( row );
         }
      }
      return null();
   }
};

#endif
