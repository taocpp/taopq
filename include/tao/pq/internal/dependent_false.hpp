// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_DEPENDENT_FALSE_HPP
#define TAO_PQ_INTERNAL_DEPENDENT_FALSE_HPP

namespace tao::pq::internal
{
   template< typename... >
   inline constexpr bool dependent_false = false;

}  // namespace tao::pq::internal

#endif
