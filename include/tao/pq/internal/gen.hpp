// Copyright (c) 2019-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_GEN_HPP
#define TAO_PQ_INTERNAL_GEN_HPP

#include <tao/pq/internal/exclusive_scan.hpp>

namespace tao::pq::internal
{
   template< typename, typename, typename >
   struct make;

   template< std::size_t... Is,
             std::size_t... Js,
             std::size_t... Ns >
   struct make< std::index_sequence< Is... >,
                std::index_sequence< Js... >,
                std::index_sequence< Ns... > >
   {
      template< std::size_t I >
      static constexpr std::size_t outer = ( 0 + ... + ( ( Ns <= I ) ? 1 : 0 ) ) - 1;

      template< std::size_t J >
      static constexpr std::size_t select = ( 0 + ... + ( ( Js == J ) ? Ns : 0 ) );

      template< std::size_t I >
      static constexpr std::size_t inner = I - select< outer< I > >;

      using outer_sequence = std::index_sequence< outer< Is >... >;
      using inner_sequence = std::index_sequence< inner< Is >... >;
   };

   template< std::size_t... Ns >
   struct gen2
   {
      using S1 = std::make_index_sequence< ( 0 + ... + Ns ) >;
      using S2 = std::make_index_sequence< sizeof...( Ns ) >;
      using S3 = exclusive_scan_t< std::index_sequence< Ns... > >;
      using type = make< S1, S2, S3 >;
   };

   template< std::size_t... Ns >
   using gen = typename gen2< Ns... >::type;

}  // namespace tao::pq::internal

#endif
