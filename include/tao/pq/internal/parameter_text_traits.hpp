// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_PARAMETER_TEXT_TRAITS_HPP
#define TAO_PQ_INTERNAL_PARAMETER_TEXT_TRAITS_HPP

#include <cstddef>
#include <string>
#include <utility>

#include <tao/pq/internal/parameter_traits_helper.hpp>
#include <tao/pq/internal/printf.hpp>

#include <libpq-fe.h>

namespace tao::pq::internal
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
      : char_pointer_helper
   {
      explicit parameter_text_traits( const bool v ) noexcept
         : char_pointer_helper( v ? "true" : "false" )
      {}
   };

   template<>
   struct parameter_text_traits< char >
      : string_helper
   {
      parameter_text_traits( const char v )
         : string_helper( 1, v )
      {}
   };

   template<>
   struct parameter_text_traits< signed char >
      : string_helper
   {
      parameter_text_traits( const signed char v )
         : string_helper( printf( "%hhd", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned char >
      : string_helper
   {
      parameter_text_traits( const unsigned char v )
         : string_helper( printf( "%hhu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< short >
      : string_helper
   {
      parameter_text_traits( const short v )
         : string_helper( printf( "%hd", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned short >
      : string_helper
   {
      parameter_text_traits( const unsigned short v )
         : string_helper( printf( "%hu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< int >
      : string_helper
   {
      parameter_text_traits( const int v )
         : string_helper( printf( "%d", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned >
      : string_helper
   {
      parameter_text_traits( const unsigned v )
         : string_helper( printf( "%u", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< long >
      : string_helper
   {
      parameter_text_traits( const long v )
         : string_helper( printf( "%ld", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned long >
      : string_helper
   {
      parameter_text_traits( const unsigned long v )
         : string_helper( printf( "%lu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< long long >
      : string_helper
   {
      parameter_text_traits( const long long v )
         : string_helper( printf( "%lld", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned long long >
      : string_helper
   {
      parameter_text_traits( const unsigned long long v )
         : string_helper( printf( "%llu", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< float >
      : string_helper
   {
      parameter_text_traits( const float v )
         : string_helper( printf( "%.9g", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< double >
      : string_helper
   {
      parameter_text_traits( const double v )
         : string_helper( printf( "%.17g", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< long double >
      : string_helper
   {
      parameter_text_traits( const long double v )
         : string_helper( printf( "%.21Lg", v ) )
      {}
   };

   template<>
   struct parameter_text_traits< std::string >
      : char_pointer_helper
   {
      explicit parameter_text_traits( const std::string& v ) noexcept
         : char_pointer_helper( v.c_str() )
      {}
   };

}  // namespace tao::pq::internal

#endif
