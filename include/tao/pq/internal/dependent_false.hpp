// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_DEPENDENT_FALSE_HPP
#define TAO_PQ_INTERNAL_DEPENDENT_FALSE_HPP

namespace tao::pq::internal
{
   template< typename... >
   inline constexpr bool dependent_false = false;

}  // namespace tao::pq::internal

#endif
