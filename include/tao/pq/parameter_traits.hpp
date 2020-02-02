// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_PARAMETER_TRAITS_HPP
#define TAO_PQ_PARAMETER_TRAITS_HPP

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/internal/gen.hpp>
#include <tao/pq/internal/parameter_traits_helper.hpp>
#include <tao/pq/null.hpp>
#include <tao/pq/parameter_text_traits.hpp>

namespace tao::pq
{
   // select which traits you want (TODO: this is just a hack, improve it!)
   template< typename T, typename = void >
   struct parameter_traits
      : parameter_text_traits< T >
   {
      using parameter_text_traits< T >::parameter_text_traits;
   };

   template<>
   struct parameter_traits< null_t >
   {
      explicit parameter_traits( const null_t& /*unused*/ ) noexcept
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr const char* value() noexcept
      {
         return nullptr;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int length() noexcept
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int format() noexcept
      {
         return 0;
      }
   };

   template<>
   struct parameter_traits< const char* >
      : internal::char_pointer_helper
   {
      explicit parameter_traits( const char* p ) noexcept
         : char_pointer_helper( p )
      {}
   };

   template<>
   struct parameter_traits< std::string >
      : internal::char_pointer_helper
   {
      explicit parameter_traits( const std::string& v ) noexcept
         : char_pointer_helper( v.c_str() )
      {}
   };

   // libpq has no way to accept a non-null-terminated string,
   // hence we are required to create a copy just to add a null-byte.
   template<>
   struct parameter_traits< std::string_view >
      : internal::string_helper
   {
      explicit parameter_traits( const std::string_view v ) noexcept
         : string_helper( v )
      {}
   };

   template< typename T >
   struct parameter_traits< std::optional< T > >
   {
   private:
      using U = parameter_traits< std::decay_t< T > >;
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
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return U::template type< I >();
      }

      template< std::size_t I >
      [[nodiscard]] constexpr const char* value() const noexcept( noexcept( m_forwarder ? m_forwarder->template value< I >() : nullptr ) )
      {
         return m_forwarder ? m_forwarder->template value< I >() : nullptr;
      }

      template< std::size_t I >
      [[nodiscard]] constexpr int length() const noexcept( noexcept( m_forwarder ? m_forwarder->template length< I >() : 0 ) )
      {
         return m_forwarder ? m_forwarder->template length< I >() : 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int format() noexcept
      {
         return U::template format< I >();
      }
   };

   template< typename... Ts >
   struct parameter_traits< std::tuple< Ts... > >
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

      template< std::size_t I >
      [[nodiscard]] constexpr Oid type() const noexcept( noexcept( std::get< gen::template outer< I > >( m_tuple ).template type< gen::template inner< I > >() ) )
      {
         return std::get< gen::template outer< I > >( m_tuple ).template type< gen::template inner< I > >();
      }

      template< std::size_t I >
      [[nodiscard]] const char* value() const noexcept( noexcept( std::get< gen::template outer< I > >( m_tuple ).template value< gen::template inner< I > >() ) )
      {
         return std::get< gen::template outer< I > >( m_tuple ).template value< gen::template inner< I > >();
      }

      template< std::size_t I >
      [[nodiscard]] constexpr int length() const noexcept( noexcept( std::get< gen::template outer< I > >( m_tuple ).template length< gen::template inner< I > >() ) )
      {
         return std::get< gen::template outer< I > >( m_tuple ).template length< gen::template inner< I > >();
      }

      template< std::size_t I >
      [[nodiscard]] constexpr int format() const noexcept( noexcept( std::get< gen::template outer< I > >( m_tuple ).template format< gen::template inner< I > >() ) )
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

   // detect free function to_taopq_param() found via ADL, simplify user-defined traits
   template< typename T >
   struct parameter_traits< T, std::void_t< decltype( to_taopq_param( std::declval< const T& >() ) ) > >
      : parameter_traits< decltype( to_taopq_param( std::declval< const T& >() ) ) >
   {
      explicit parameter_traits( const T& t ) noexcept( noexcept( parameter_traits< decltype( to_taopq_param( t ) ) >( to_taopq_param( t ) ) ) )
         : parameter_traits< decltype( to_taopq_param( std::declval< const T& >() ) ) >( to_taopq_param( t ) )
      {}
   };

}  // namespace tao::pq

#endif
