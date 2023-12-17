// Copyright (c) 2021-2023 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_PARAMETER_TRAITS_TUPLE_HPP
#define TAO_PQ_PARAMETER_TRAITS_TUPLE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/gen.hpp>
#include <tao/pq/parameter_traits.hpp>

template< typename... Ts >
struct tao::pq::parameter_traits< std::tuple< Ts... > >
{
private:
   using tuple_t = std::tuple< parameter_traits< std::decay_t< Ts > >... >;
   tuple_t m_tuple;

   using gen = internal::gen< parameter_traits< std::decay_t< Ts > >::columns... >;

public:
   explicit parameter_traits( const std::tuple< Ts... >& tuple ) noexcept( noexcept( tuple_t( tuple ) ) )
      : m_tuple( tuple )
   {}

   explicit parameter_traits( std::tuple< Ts... >&& tuple ) noexcept( noexcept( tuple_t( std::move( tuple ) ) ) )
      : m_tuple( std::move( tuple ) )
   {}

   static constexpr std::size_t columns = ( 0 + ... + parameter_traits< std::decay_t< Ts > >::columns );
   static constexpr bool self_contained = ( parameter_traits< std::decay_t< Ts > >::self_contained && ... );

   template< std::size_t I >
   [[nodiscard]] constexpr auto type() const noexcept( noexcept( std::get< gen::template outer< I > >( m_tuple ).template type< gen::template inner< I > >() ) ) -> oid
   {
      return std::get< gen::template outer< I > >( m_tuple ).template type< gen::template inner< I > >();
   }

   template< std::size_t I >
   [[nodiscard]] constexpr auto value() const noexcept( noexcept( std::get< gen::template outer< I > >( m_tuple ).template value< gen::template inner< I > >() ) ) -> const char*
   {
      return std::get< gen::template outer< I > >( m_tuple ).template value< gen::template inner< I > >();
   }

   template< std::size_t I >
   [[nodiscard]] constexpr auto length() const noexcept( noexcept( std::get< gen::template outer< I > >( m_tuple ).template length< gen::template inner< I > >() ) ) -> int
   {
      return std::get< gen::template outer< I > >( m_tuple ).template length< gen::template inner< I > >();
   }

   template< std::size_t I >
   [[nodiscard]] constexpr auto format() const noexcept( noexcept( std::get< gen::template outer< I > >( m_tuple ).template format< gen::template inner< I > >() ) ) -> int
   {
      return std::get< gen::template outer< I > >( m_tuple ).template format< gen::template inner< I > >();
   }

   template< std::size_t I >
   void copy_to( std::string& data ) const
   {
      std::get< gen::template outer< I > >( m_tuple ).template copy_to< gen::template inner< I > >( data );
   }
};

#endif
