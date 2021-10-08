// Copyright (c) 2019-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

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

#if defined( _WIN32 )

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

#else

   template< std::size_t... Ns >
   using gen = make< std::make_index_sequence< ( 0 + ... + Ns ) >,
                     std::make_index_sequence< sizeof...( Ns ) >,
                     exclusive_scan_t< std::index_sequence< Ns... > > >;

#endif

}  // namespace tao::pq::internal

#endif
