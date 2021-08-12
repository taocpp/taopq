// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_PARAMETER_TEXT_TRAITS_HPP
#define TAO_PQ_INTERNAL_PARAMETER_TEXT_TRAITS_HPP

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <tao/pq/binary.hpp>
#include <tao/pq/internal/dependent_false.hpp>
#include <tao/pq/internal/parameter_traits_helper.hpp>

#include <libpq-fe.h>

namespace tao::pq::internal
{
   template< std::size_t N, typename T >
   void snprintf( char ( &buffer )[ N ], const char* format, const T v )
   {
      static_assert( N >= 32 );
      if constexpr( std::is_floating_point_v< T > ) {
         if( std::isfinite( v ) ) {
            [[maybe_unused]] const auto result = std::snprintf( buffer, N, format, v );
            assert( result > 0 );
            assert( static_cast< std::size_t >( result ) < N );
         }
         else if( std::isnan( v ) ) {
#if defined( _MSC_VER )
            [[maybe_unused]] const auto result = strncpy_s( buffer, N, "NAN", N );
            assert( result == 0 );
#else
            std::strncpy( buffer, "NAN", N );
#endif
         }
         else {
#if defined( _MSC_VER )
            [[maybe_unused]] const auto result = strncpy_s( buffer, N, ( v < 0 ) ? "-INF" : "INF", N );
            assert( result == 0 );
#else
            std::strncpy( buffer, ( v < 0 ) ? "-INF" : "INF", N );
#endif
         }
      }
      else {
         [[maybe_unused]] const auto result = std::snprintf( buffer, N, format, v );
         assert( result > 0 );
         assert( static_cast< std::size_t >( result ) < N );
      }
   }

   template< typename T, typename = void >
   struct parameter_text_traits
   {
      static_assert( dependent_false< T >, "data type T not registered as taopq parameter" );

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

      template< std::size_t I >
      [[nodiscard]] static constexpr auto string_view() noexcept -> std::string_view
      {
         return {};
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto escape() noexcept -> bool
      {
         return false;
      }
   };

   template<>
   struct parameter_text_traits< bool >
      : char_pointer_helper
   {
      explicit parameter_text_traits( const bool v ) noexcept
         : char_pointer_helper( v ? "true" : "false" )
      {}

      template< std::size_t I >
      [[nodiscard]] static constexpr auto escape() noexcept -> bool
      {
         return false;
      }
   };

   template<>
   struct parameter_text_traits< char >
      : string_helper
   {
      parameter_text_traits( const char v )
         : string_helper( 1, v )
      {}
   };

   struct buffer_helper
   {
      char m_buffer[ 32 ];

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return m_buffer;
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

      template< std::size_t I >
      [[nodiscard]] auto string_view() const noexcept -> std::string_view
      {
         return m_buffer;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto escape() noexcept -> bool
      {
         return false;
      }
   };

   template<>
   struct parameter_text_traits< signed char >
      : buffer_helper
   {
      parameter_text_traits( const signed char v )
      {
         snprintf( m_buffer, "%hhd", v );
      }
   };

   template<>
   struct parameter_text_traits< unsigned char >
      : buffer_helper
   {
      parameter_text_traits( const unsigned char v )
      {
         snprintf( m_buffer, "%hhu", v );
      }
   };

   template<>
   struct parameter_text_traits< short >
      : buffer_helper
   {
      parameter_text_traits( const short v )
      {
         snprintf( m_buffer, "%hd", v );
      }
   };

   template<>
   struct parameter_text_traits< unsigned short >
      : buffer_helper
   {
      parameter_text_traits( const unsigned short v )
      {
         snprintf( m_buffer, "%hu", v );
      }
   };

   template<>
   struct parameter_text_traits< int >
      : buffer_helper
   {
      parameter_text_traits( const int v )
      {
         snprintf( m_buffer, "%d", v );
      }
   };

   template<>
   struct parameter_text_traits< unsigned >
      : buffer_helper
   {
      parameter_text_traits( const unsigned v )
      {
         snprintf( m_buffer, "%u", v );
      }
   };

   template<>
   struct parameter_text_traits< long >
      : buffer_helper
   {
      parameter_text_traits( const long v )
      {
         snprintf( m_buffer, "%ld", v );
      }
   };

   template<>
   struct parameter_text_traits< unsigned long >
      : buffer_helper
   {
      parameter_text_traits( const unsigned long v )
      {
         snprintf( m_buffer, "%lu", v );
      }
   };

   template<>
   struct parameter_text_traits< long long >
      : buffer_helper
   {
      parameter_text_traits( const long long v )
      {
         snprintf( m_buffer, "%lld", v );
      }
   };

   template<>
   struct parameter_text_traits< unsigned long long >
      : buffer_helper
   {
      parameter_text_traits( const unsigned long long v )
      {
         snprintf( m_buffer, "%llu", v );
      }
   };

   template<>
   struct parameter_text_traits< float >
      : buffer_helper
   {
      parameter_text_traits( const float v )
      {
         snprintf( m_buffer, "%.9g", v );
      }
   };

   template<>
   struct parameter_text_traits< double >
      : buffer_helper
   {
      parameter_text_traits( const double v )
      {
         snprintf( m_buffer, "%.17g", v );
      }
   };

   template<>
   struct parameter_text_traits< long double >
      : buffer_helper
   {
      parameter_text_traits( const long double v )
      {
         snprintf( m_buffer, "%.21Lg", v );
      }
   };

   template<>
   struct parameter_text_traits< binary_view >
   {
   private:
      unsigned char* m_data;

   public:
      parameter_text_traits( PGconn* c, const binary_view v, std::size_t dummy = 0 )
         : m_data( PQescapeByteaConn( c, (unsigned char*)v.data(), v.size(), &dummy ) )  // NOLINT
      {
         if( m_data == nullptr ) {
            throw std::bad_alloc();  // LCOV_EXCL_LINE
         }
      }

      parameter_text_traits( const parameter_text_traits& ) = delete;
      parameter_text_traits( parameter_text_traits&& ) = delete;

      ~parameter_text_traits()
      {
         PQfreemem( m_data );
      }

      void operator=( const parameter_text_traits& ) = delete;
      void operator=( parameter_text_traits&& ) = delete;

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( m_data );
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

      template< std::size_t I >
      [[nodiscard]] auto string_view() const noexcept -> std::string_view
      {
         return value< I >();
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto escape() noexcept -> bool
      {
         return false;
      }
   };

}  // namespace tao::pq::internal

#endif
