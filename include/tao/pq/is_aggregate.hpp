// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_IS_AGGREGATE_HPP
#define TAO_PQ_IS_AGGREGATE_HPP

namespace tao::pq
{
   template< typename >
   inline constexpr bool is_aggregate = false;

   template< typename T >
   inline constexpr bool is_aggregate_result = is_aggregate< T >;

   template< typename T >
   inline constexpr bool is_aggregate_parameter = is_aggregate< T >;

}  // namespace tao::pq

#endif
