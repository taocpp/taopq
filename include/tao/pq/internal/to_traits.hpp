// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_TO_TRAITS_HPP
#define TAO_PQ_INTERNAL_TO_TRAITS_HPP

#include <type_traits>
#include <utility>

#include <tao/pq/fwd.hpp>

namespace tao::pq::internal
{
   template< typename A >
   [[nodiscard]] auto to_traits( A&& a )
   {
      return parameter_traits< std::decay_t< A > >( std::forward< A >( a ) );
   }

}  // namespace tao::pq::internal

#endif
