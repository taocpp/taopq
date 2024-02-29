// Copyright (c) 2020-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_PARAMETER_TRAITS_AGGREGATE_HPP
#define TAO_PQ_PARAMETER_TRAITS_AGGREGATE_HPP

#include <tao/pq/internal/aggregate.hpp>
#include <tao/pq/is_aggregate.hpp>
#include <tao/pq/parameter_traits.hpp>

namespace tao::pq
{
   namespace internal
   {
      template< typename T >
      struct parameter_tie_aggregate
      {
         using result_t = decltype( internal::tie_aggregate( std::declval< const T& >() ) );
         const result_t result;

         explicit parameter_tie_aggregate( const T& t ) noexcept
            : result( internal::tie_aggregate( t ) )
         {}
      };

   }  // namespace internal

   template< typename T >
   struct parameter_traits< T, std::enable_if_t< is_aggregate_parameter< T > > >
      : private internal::parameter_tie_aggregate< T >,
        public parameter_traits< typename internal::parameter_tie_aggregate< T >::result_t >
   {
      using typename internal::parameter_tie_aggregate< T >::result_t;

      explicit parameter_traits( const T& t ) noexcept( noexcept( internal::parameter_tie_aggregate< T >( t ),
                                                                  parameter_traits< result_t >( std::declval< result_t >() ) ) )
         : internal::parameter_tie_aggregate< T >( t ),
           parameter_traits< result_t >( this->result )
      {}
   };

}  // namespace tao::pq

#endif
