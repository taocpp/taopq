// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_TO_TRAITS_HPP
#define TAO_PQ_INTERNAL_TO_TRAITS_HPP

#include <type_traits>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/internal/dependent_false.hpp>

namespace tao::pq::internal
{
   template< template< typename... > class Traits >
   struct wrap_traits
   {};

   template< template< typename... > class Traits, typename A >
   [[nodiscard]] auto to_traits( [[maybe_unused]] PGconn* c, A&& a )
   {
      using T = Traits< std::decay_t< A > >;
      if constexpr( std::is_constructible_v< T, decltype( std::forward< A >( a ) ) > ) {
         return T( std::forward< A >( a ) );
      }
      else if constexpr( std::is_constructible_v< T, PGconn*, decltype( std::forward< A >( a ) ) > ) {
         return T( c, std::forward< A >( a ) );
      }
      else if constexpr( std::is_constructible_v< T, PGconn*, decltype( std::forward< A >( a ) ), wrap_traits< Traits > > ) {
         return T( c, std::forward< A >( a ), wrap_traits< Traits >() );
      }
      else {
         static_assert( dependent_false< A >, "no valid conversion from A via Traits" );
      }
   }

}  // namespace tao::pq::internal

#endif
