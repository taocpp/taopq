// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_PARAMETER_TRAITS_HPP
#define TAO_PQ_PARAMETER_TRAITS_HPP

#include <tao/pq/internal/parameter_binary_traits.hpp>
#include <tao/pq/internal/parameter_text_traits.hpp>
#include <tao/pq/internal/parameter_traits.hpp>

namespace tao::pq
{
   template< typename T, typename = void >
   struct parameter_text_traits
      : internal::parameter_traits< internal::parameter_text_traits, T >
   {
      using internal::parameter_traits< internal::parameter_text_traits, T >::parameter_traits;
   };

   template< typename T, typename = void >
   struct parameter_binary_traits
      : internal::parameter_traits< internal::parameter_binary_traits, T >
   {
      using internal::parameter_traits< internal::parameter_binary_traits, T >::parameter_traits;
   };

}  // namespace tao::pq

#endif
