// Copyright (c) 2019-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_PARAMETER_BINARY_TRAITS_HPP
#define TAO_PQ_INTERNAL_PARAMETER_BINARY_TRAITS_HPP

#include <cstddef>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/internal/dependent_false.hpp>
#include <tao/pq/internal/endian.hpp>
#include <tao/pq/internal/is_bytea_parameter.hpp>
#include <tao/pq/span.hpp>

namespace tao::pq::internal
{
   template< typename T, typename = void >
   struct parameter_binary_traits
   {
      static constexpr std::size_t columns = 1;

      static_assert( dependent_false< T >, "data type T not registered as taopq parameter" );

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

   template<>
   struct parameter_binary_traits< bool >
   {
      const bool m_v;

      explicit parameter_binary_traits( const bool v ) noexcept
         : m_v( v )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 16;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< char >
   {
      const char m_v;

      explicit parameter_binary_traits( const char v ) noexcept
         : m_v( v )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 18;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< short >
   {
      const short m_v;

      static_assert( sizeof( short ) == 2 );

      explicit parameter_binary_traits( const short v ) noexcept
         : m_v( hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 21;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( short );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< int >
   {
      const int m_v;

      static_assert( sizeof( int ) == 4 );

      explicit parameter_binary_traits( const int v ) noexcept
         : m_v( hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 23;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( int );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< long >
   {
      const long m_v;

      static_assert( ( sizeof( long ) == 4 ) || ( sizeof( long ) == 8 ) );

      explicit parameter_binary_traits( const long v ) noexcept
         : m_v( hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return ( sizeof( long ) == 4 ) ? 23 : 20;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( long );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< long long >
   {
      const long long m_v;

      static_assert( sizeof( long long ) == 8 );

      explicit parameter_binary_traits( const long long v ) noexcept
         : m_v( hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 20;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( long long );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< float >
   {
      const float m_v;

      static_assert( sizeof( float ) == 4 );

      explicit parameter_binary_traits( const float v ) noexcept
         : m_v( hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 700;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( float );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< double >
   {
      const double m_v;

      static_assert( sizeof( double ) == 8 );

      explicit parameter_binary_traits( const double v ) noexcept
         : m_v( hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 701;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( double );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< std::string_view >
   {
   private:
      const std::string_view m_v;

   protected:
      explicit parameter_binary_traits( const std::string_view v ) noexcept
         : m_v( v )
      {}

   public:
      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 25;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return m_v.data();
      }

      template< std::size_t I >
      [[nodiscard]] auto length() const noexcept -> int
      {
         return static_cast< int >( m_v.size() );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< std::string >
      : parameter_binary_traits< std::string_view >
   {
      explicit parameter_binary_traits( const std::string& v ) noexcept
         : parameter_binary_traits< std::string_view >( v )
      {}
   };

   template< typename ElementType, std::size_t Extent >
   struct parameter_binary_traits< tao::pq::span< ElementType, Extent >, std::enable_if_t< is_bytea_parameter< ElementType > > >
   {
   private:
      const tao::pq::span< const ElementType, Extent > m_v;

   public:
      explicit parameter_binary_traits( const tao::pq::span< const ElementType, Extent > v ) noexcept
         : m_v( v )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 17;
      }

      template< std::size_t I >
      [[nodiscard]] constexpr auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( m_v.data() );
      }

      template< std::size_t I >
      [[nodiscard]] constexpr auto length() const noexcept -> int
      {
         return static_cast< int >( m_v.size() );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

}  // namespace tao::pq::internal

#endif
