// Copyright (c) 2019-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_PARAMETER_BINARY_TRAITS_HPP
#define TAO_PQ_PARAMETER_BINARY_TRAITS_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

#include <byteswap.h>
#include <cstring>

#include <libpq-fe.h>

namespace tao::pq
{
   namespace internal
   {
      template< typename To, typename From >
      std::enable_if_t< ( sizeof( To ) == sizeof( From ) ) && std::is_trivially_copyable< From >::value && std::is_trivial< To >::value, To >
      bit_cast( const From& src ) noexcept
      {
         To dst;
         std::memcpy( &dst, &src, sizeof( To ) );
         return dst;
      }

   }  // namespace internal

   template< typename T >
   struct parameter_binary_traits
   {
      static constexpr std::size_t columns = 1;

      static_assert( sizeof( T ) == 0, "data type T not registered as taopq parameter" );

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
   struct parameter_binary_traits< bool >
   {
      const bool m_v;

      explicit parameter_binary_traits( const bool v ) noexcept
         : m_v( v )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 16;
      }

      template< std::size_t I >
      [[nodiscard]] const char* value() const noexcept
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int length() noexcept
      {
         return 1;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int format() noexcept
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
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 18;
      }

      template< std::size_t I >
      [[nodiscard]] const char* value() const noexcept
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int length() noexcept
      {
         return 1;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int format() noexcept
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
         : m_v( bswap_16( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 21;
      }

      template< std::size_t I >
      [[nodiscard]] const char* value() const noexcept
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int length() noexcept
      {
         return sizeof( short );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int format() noexcept
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
         : m_v( bswap_32( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 23;
      }

      template< std::size_t I >
      [[nodiscard]] const char* value() const noexcept
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int length() noexcept
      {
         return sizeof( int );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int format() noexcept
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< long >
   {
      const long m_v;

      static_assert( sizeof( long ) == 8 );

      explicit parameter_binary_traits( const long v ) noexcept
         : m_v( bswap_64( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 20;
      }

      template< std::size_t I >
      [[nodiscard]] const char* value() const noexcept
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int length() noexcept
      {
         return sizeof( long );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int format() noexcept
      {
         return 1;
      }
   };

   template<>
   struct parameter_binary_traits< long long >
      : parameter_binary_traits< long >
   {
      using parameter_binary_traits< long >::parameter_binary_traits;
      static_assert( sizeof( long long ) == 8 );
   };

   template<>
   struct parameter_binary_traits< float >
      : parameter_binary_traits< int >
   {
      explicit parameter_binary_traits( const float v ) noexcept
         : parameter_binary_traits< int >( internal::bit_cast< int >( v ) )
      {
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 700;
      }
   };

   template<>
   struct parameter_binary_traits< double >
      : parameter_binary_traits< long >
   {
      explicit parameter_binary_traits( const double v )
         : parameter_binary_traits< long >( internal::bit_cast< long >( v ) )
      {}

      template< std::size_t I >
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 701;
      }
   };

}  // namespace tao::pq

#endif
