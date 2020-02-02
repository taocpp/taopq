// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_PARAMETER_TEXT_TRAITS_HPP
#define TAO_PQ_PARAMETER_TEXT_TRAITS_HPP

#include <cstddef>
#include <utility>

#include <tao/pq/internal/parameter_traits_helper.hpp>
#include <tao/pq/internal/printf.hpp>

#include <libpq-fe.h>

namespace tao::pq
{
   template< typename T >
   struct parameter_text_traits
   {
      static_assert( sizeof( T ) == 0, "data type T not registered as taopq parameter" );

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
   struct parameter_text_traits< bool >
      : internal::char_pointer_helper
   {
      explicit parameter_text_traits( const bool v ) noexcept
         : char_pointer_helper( v ? "true" : "false" )
      {}
   };

   template<>
   struct parameter_text_traits< char >
      : internal::string_helper
   {
      parameter_text_traits( const char v )
         : string_helper( 1, v )
      {}
   };

   template<>
   struct parameter_text_traits< signed char >
      : internal::string_helper
   {
      parameter_text_traits( const signed char v )
         : string_helper( internal::printf( "%hhd", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned char >
      : internal::string_helper
   {
      parameter_text_traits( const unsigned char v )
         : string_helper( internal::printf( "%hhu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< short >
      : internal::string_helper
   {
      parameter_text_traits( const short v )
         : string_helper( internal::printf( "%hd", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned short >
      : internal::string_helper
   {
      parameter_text_traits( const unsigned short v )
         : string_helper( internal::printf( "%hu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< int >
      : internal::string_helper
   {
      parameter_text_traits( const int v )
         : string_helper( internal::printf( "%d", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned >
      : internal::string_helper
   {
      parameter_text_traits( const unsigned v )
         : string_helper( internal::printf( "%u", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< long >
      : internal::string_helper
   {
      parameter_text_traits( const long v )
         : string_helper( internal::printf( "%ld", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned long >
      : internal::string_helper
   {
      parameter_text_traits( const unsigned long v )
         : string_helper( internal::printf( "%lu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< long long >
      : internal::string_helper
   {
      parameter_text_traits( const long long v )
         : string_helper( internal::printf( "%lld", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned long long >
      : internal::string_helper
   {
      parameter_text_traits( const unsigned long long v )
         : string_helper( internal::printf( "%llu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< float >
      : internal::string_helper
   {
      parameter_text_traits( const float v )
         : string_helper( internal::printf( "%.9g", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< double >
      : internal::string_helper
   {
      parameter_text_traits( const double v )
         : string_helper( internal::printf( "%.17g", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< long double >
      : internal::string_helper
   {
      parameter_text_traits( const long double v )
         : string_helper( internal::printf( "%.21Lg", v ) )
      {}
   };

}  // namespace tao::pq

#endif
