// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_NULL_HPP
#define TAO_PQ_NULL_HPP

namespace tao::pq
{
   struct null_t final
   {
      explicit constexpr null_t( int /*unused*/ ) {}
   };

   inline constexpr null_t null{ 0 };

}  // namespace tao::pq

#endif
