// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_PARAMETER_TRAITS_HPP
#define TAO_PQ_INTERNAL_PARAMETER_TRAITS_HPP

#include <cstddef>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/internal/gen.hpp>
#include <tao/pq/internal/parameter_traits_helper.hpp>
#include <tao/pq/null.hpp>

namespace tao::pq::internal
{
   template< template< typename... > class Traits, typename T, typename = void >
   struct parameter_traits
      : Traits< T >
   {
      using Traits< T >::Traits;
   };

   template< template< typename... > class Traits >
   struct parameter_traits< Traits, null_t >
   {
      explicit parameter_traits( const null_t& /*unused*/ ) noexcept
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto value() noexcept -> const char*
      {
         return nullptr;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 0;
      }
   };

   template< template< typename... > class Traits >
   struct parameter_traits< Traits, const char* >
      : char_pointer_helper
   {
      explicit parameter_traits( const char* p ) noexcept
         : char_pointer_helper( p )
      {}
   };

   template< template< typename... > class Traits, typename T >
   struct parameter_traits< Traits, std::optional< T > >
   {
   private:
      using U = parameter_traits< Traits, std::decay_t< T > >;
      std::optional< U > m_forwarder;

   public:
      explicit parameter_traits( const std::optional< T >& v )
      {
         if( v ) {
            m_forwarder.emplace( *v );
         }
      }

      explicit parameter_traits( std::optional< T >&& v )
      {
         if( v ) {
            m_forwarder.emplace( std::move( *v ) );
         }
      }

      static constexpr std::size_t columns = U::columns;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
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
   };

   template< template< typename... > class Traits, typename... Ts >
   struct parameter_traits< Traits, std::tuple< Ts... > >
   {
   private:
      using tuple_t = std::tuple< parameter_traits< Traits, std::decay_t< Ts > >... >;
      tuple_t m_tuple;

      using gen = internal::gen< parameter_traits< Traits, std::decay_t< Ts > >::columns... >;

   public:
      explicit parameter_traits( const std::tuple< Ts... >& tuple ) noexcept( noexcept( tuple_t( tuple ) ) )
         : m_tuple( tuple )
      {}

      explicit parameter_traits( std::tuple< Ts... >&& tuple ) noexcept( noexcept( tuple_t( std::move( tuple ) ) ) )
         : m_tuple( std::move( tuple ) )
      {}

      static constexpr std::size_t columns{ ( 0 + ... + parameter_traits< Traits, std::decay_t< Ts > >::columns ) };

      template< std::size_t I >
      [[nodiscard]] constexpr auto type() const noexcept( noexcept( std::get< gen::template outer< I > >( m_tuple ).template type< gen::template inner< I > >() ) ) -> Oid
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
   };

   // default free function to detect member function to_taopq_param()
   template< typename T >
   auto to_taopq_param( const T& t ) noexcept( noexcept( t.to_taopq_param() ) )
      -> decltype( t.to_taopq_param() )
   {
      return t.to_taopq_param();
   }

   template< typename T >
   struct parameter_holder
   {
      using result_t = decltype( to_taopq_param( std::declval< const T& >() ) );
      const result_t result;

      explicit parameter_holder( const T& t ) noexcept( noexcept( result_t( to_taopq_param( t ) ) ) )
         : result( to_taopq_param( t ) )
      {}
   };

   template< template< typename... > class Traits, typename T >
   struct parameter_traits< Traits, T, std::void_t< decltype( to_taopq_param( std::declval< const T& >() ) ) > >
      : private parameter_holder< T >, public parameter_traits< Traits, typename parameter_holder< T >::result_t >
   {
      using typename parameter_holder< T >::result_t;

      explicit parameter_traits( const T& t ) noexcept( noexcept( parameter_holder< T >( t ), parameter_traits< Traits, result_t >( std::declval< result_t >() ) ) )
         : parameter_holder< T >( t ),
           parameter_traits< Traits, result_t >( this->result )
      {}
   };

}  // namespace tao::pq::internal

#endif
