// Copyright (c) 2021-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_PARAMETER_TRAITS_OPTIONAL_HPP
#define TAO_PQ_PARAMETER_TRAITS_OPTIONAL_HPP

#include <optional>
#include <type_traits>
#include <utility>

#include <tao/pq/parameter_traits.hpp>

template< typename T >
struct tao::pq::parameter_traits< std::optional< T > >
{
private:
   using U = parameter_traits< std::decay_t< T > >;
   std::optional< U > m_forwarder;

public:
   explicit parameter_traits( const std::optional< T >& v ) noexcept( noexcept( m_forwarder.emplace( *v ) ) )
   {
      if( v ) {
         m_forwarder.emplace( *v );
      }
   }

   explicit parameter_traits( std::optional< T >&& v ) noexcept( noexcept( m_forwarder.emplace( std::move( *v ) ) ) )
   {
      if( v ) {
         m_forwarder.emplace( std::move( *v ) );
      }
   }

   static constexpr std::size_t columns = U::columns;

   template< std::size_t I >
   [[nodiscard]] static constexpr auto type() noexcept -> oid
   {
      return U::template type< I >();
   }

   template< std::size_t I >
   [[nodiscard]] constexpr auto value() const noexcept( noexcept( m_forwarder ? m_forwarder->template value< I >() : nullptr ) ) -> const char*
   {
      return m_forwarder ? m_forwarder->template value< I >() : nullptr;
   }

   template< std::size_t I >
   [[nodiscard]] constexpr auto length() const noexcept( noexcept( m_forwarder ? m_forwarder->template length< I >() : 0 ) ) -> int
   {
      return m_forwarder ? m_forwarder->template length< I >() : 0;
   }

   template< std::size_t I >
   [[nodiscard]] static constexpr auto format() noexcept -> int
   {
      return U::template format< I >();
   }

   template< std::size_t I >
   void element( std::string& data ) const
   {
      if( m_forwarder ) {
         m_forwarder->template element< I >( data );
      }
      else {
         data += "NULL";
      }
   }

   template< std::size_t I >
   void copy_to( std::string& data ) const
   {
      if( m_forwarder ) {
         m_forwarder->template copy_to< I >( data );
      }
      else {
         data += "\\N";
      }
   }
};

#endif
