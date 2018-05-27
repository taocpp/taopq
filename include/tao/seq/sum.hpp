// Copyright (c) 2015-2018 Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/sequences/

#ifndef TAO_SEQ_SUM_HPP
#define TAO_SEQ_SUM_HPP

#include <type_traits>
#include <utility>

#include "config.hpp"
#include "integer_sequence.hpp"

#ifndef TAO_FOLD_EXPRESSIONS
#include "make_integer_sequence.hpp"
#include <cstddef>
#endif

namespace tao
{
   namespace seq
   {

#ifdef TAO_FOLD_EXPRESSIONS

      template< typename T, T... Ns >
      struct sum
         : std::integral_constant< T, ( Ns + ... + T( 0 ) ) >
      {
      };

#else

      namespace impl
      {
         template< std::size_t, std::size_t N >
         struct chars
         {
            char dummy[ N + 1 ];
         };

         template< typename, std::size_t... >
         struct collector;

         template< std::size_t... Is, std::size_t... Ns >
         struct collector< index_sequence< Is... >, Ns... >
            : chars< Is, Ns >...
         {
         };

         template< typename T, T v >
         struct unary_minus_t
         {
            using type = std::integral_constant< T, -v >;
         };

         template< typename T, T v >
         using unary_minus = typename unary_minus_t< T, v >::type;

         template< bool, std::size_t N, typename T, T... Ns >
         struct sum
         {
            using type = std::integral_constant< T, T( sizeof( collector< make_index_sequence< N >, ( ( Ns > 0 ) ? Ns : 0 )... > ) ) - T( sizeof( collector< make_index_sequence< N >, ( ( Ns < 0 ) ? unary_minus< T, Ns >::value : 0 )... > ) ) >;
         };

         template< std::size_t N, typename T, T... Ns >
         struct sum< true, N, T, Ns... >
         {
            using type = std::integral_constant< T, T( sizeof( collector< make_index_sequence< N >, ( ( Ns > 0 ) ? Ns : 0 )... > ) - N ) >;
         };

      }  // namespace impl

      template< typename T, T... Ns >
      struct sum
         : impl::sum< std::is_unsigned< T >::value, sizeof...( Ns ) + 1, T, Ns..., 0 >::type
      {
      };

#endif

      template< typename T, T... Ns >
      struct sum< integer_sequence< T, Ns... > >
         : sum< T, Ns... >
      {
      };

   }  // namespace seq

}  // namespace tao

#endif
