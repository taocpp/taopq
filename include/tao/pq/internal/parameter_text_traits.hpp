// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_PARAMETER_TEXT_TRAITS_HPP
#define TAO_PQ_INTERNAL_PARAMETER_TEXT_TRAITS_HPP

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <tao/pq/binary.hpp>
#include <tao/pq/internal/dependent_false.hpp>
#include <tao/pq/internal/parameter_traits_helper.hpp>
#include <tao/pq/internal/printf.hpp>

#include <libpq-fe.h>

namespace tao::pq::internal
{
   template< typename T >
   [[nodiscard]] auto printf_helper( const char* format, const T v ) -> std::string
   {
      if( std::isfinite( v ) ) {
         return printf( format, v );
      }
      if( std::isnan( v ) ) {
         return "NAN";
      }
      return ( v < 0 ) ? "-INF" : "INF";
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

   struct safe_string_helper
      : string_helper
   {
      using string_helper::string_helper;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto escape() noexcept -> bool
      {
         return false;
      }
   };

   template<>
   struct parameter_text_traits< signed char >
      : safe_string_helper
   {
      parameter_text_traits( const signed char v )
         : safe_string_helper( printf( "%hhd", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned char >
      : safe_string_helper
   {
      parameter_text_traits( const unsigned char v )
         : safe_string_helper( printf( "%hhu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< short >
      : safe_string_helper
   {
      parameter_text_traits( const short v )
         : safe_string_helper( printf( "%hd", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned short >
      : safe_string_helper
   {
      parameter_text_traits( const unsigned short v )
         : safe_string_helper( printf( "%hu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< int >
      : safe_string_helper
   {
      parameter_text_traits( const int v )
         : safe_string_helper( printf( "%d", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned >
      : safe_string_helper
   {
      parameter_text_traits( const unsigned v )
         : safe_string_helper( printf( "%u", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< long >
      : safe_string_helper
   {
      parameter_text_traits( const long v )
         : safe_string_helper( printf( "%ld", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned long >
      : safe_string_helper
   {
      parameter_text_traits( const unsigned long v )
         : safe_string_helper( printf( "%lu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< long long >
      : safe_string_helper
   {
      parameter_text_traits( const long long v )
         : safe_string_helper( printf( "%lld", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned long long >
      : safe_string_helper
   {
      parameter_text_traits( const unsigned long long v )
         : safe_string_helper( printf( "%llu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< float >
      : safe_string_helper
   {
      parameter_text_traits( const float v )
         : safe_string_helper( printf_helper( "%.9g", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< double >
      : safe_string_helper
   {
      parameter_text_traits( const double v )
         : safe_string_helper( printf_helper( "%.17g", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< long double >
      : safe_string_helper
   {
      parameter_text_traits( const long double v )
         : safe_string_helper( printf_helper( "%.21Lg", v ) )
      {}
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
