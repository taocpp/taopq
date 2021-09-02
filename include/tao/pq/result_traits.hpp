// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_RESULT_TRAITS_HPP
#define TAO_PQ_RESULT_TRAITS_HPP

#include <cstddef>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/binary.hpp>
#include <tao/pq/internal/dependent_false.hpp>

namespace tao::pq
{
   template< typename T, typename = void >
   struct result_traits
   {
      static_assert( internal::dependent_false< T >, "tao::pq::result_traits<T> not specialized for T" );
      static auto from( const char* value ) noexcept -> T;
   };

   template< typename T, typename = const std::size_t >
   inline constexpr std::size_t result_traits_size = 1;

   template< typename T >
   inline constexpr std::size_t result_traits_size< T, decltype( result_traits< T >::size ) > = result_traits< T >::size;

   template< typename T, typename = void >
   inline constexpr bool result_traits_has_null = false;

   template< typename T >
   inline constexpr bool result_traits_has_null< T, decltype( (void)result_traits< T >::null() ) > = true;

   namespace internal
   {
      template< typename T >
      auto result_traits_from( const char* value ) noexcept( noexcept( result_traits< T >::from( value ) ) )
      {
         return result_traits< T >::from( value );
      }

      template< typename T, typename = void >
      inline constexpr bool result_traits_from_string_view = false;

      template< typename T >
      inline constexpr bool result_traits_from_string_view< T, decltype( (void)result_traits< T >::from( std::declval< std::string_view >() ) ) > = true;

      template< typename T >
      auto result_traits_from( const std::string_view value ) noexcept( noexcept( result_traits< T >::from( value ) ) )
         -> std::enable_if_t< result_traits_from_string_view< T >, decltype( result_traits< T >::from( value ) ) >
      {
         return result_traits< T >::from( value );
      }

      // DANGER! This expects the string_view to be null-terminated!
      // This is generally *not* the case, only in the context of table_row!
      template< typename T >
      auto result_traits_from( const std::string_view value ) noexcept( noexcept( result_traits< T >::from( value.data() ) ) )
         -> std::enable_if_t< !result_traits_from_string_view< T >, decltype( result_traits< T >::from( value.data() ) ) >
      {
         return result_traits< T >::from( value.data() );
      }

   }  // namespace internal

   template<>
   struct result_traits< const char* >
   {
      [[nodiscard]] static auto from( const char* value )
      {
         return value;
      }
   };

   template<>
   struct result_traits< std::string_view >
   {
      [[nodiscard]] static auto from( const std::string_view value )
      {
         return value;
      }
   };

   template<>
   struct result_traits< std::string >
   {
      [[nodiscard]] static auto from( const std::string_view value )
      {
         return std::string( value );
      }
   };

   template<>
   struct result_traits< std::basic_string< unsigned char > >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> std::basic_string< unsigned char >;
   };

   template<>
   struct result_traits< binary >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> binary;
   };

   template<>
   struct result_traits< bool >
   {
      [[nodiscard]] static auto from( const char* value ) -> bool;
   };

   template<>
   struct result_traits< char >
   {
      [[nodiscard]] static auto from( const char* value ) -> char;
   };

   template<>
   struct result_traits< signed char >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> signed char;
   };

   template<>
   struct result_traits< unsigned char >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> unsigned char;
   };

   template<>
   struct result_traits< short >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> short;
   };

   template<>
   struct result_traits< unsigned short >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> unsigned short;
   };

   template<>
   struct result_traits< int >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> int;
   };

   template<>
   struct result_traits< unsigned >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> unsigned;
   };

   template<>
   struct result_traits< long >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> long;
   };

   template<>
   struct result_traits< unsigned long >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> unsigned long;
   };

   template<>
   struct result_traits< long long >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> long long;
   };

   template<>
   struct result_traits< unsigned long long >
   {
      [[nodiscard]] static auto from( const std::string_view value ) -> unsigned long long;
   };

   template<>
   struct result_traits< float >
   {
      [[nodiscard]] static auto from( const char* value ) -> float;
   };

   template<>
   struct result_traits< double >
   {
      [[nodiscard]] static auto from( const char* value ) -> double;
   };

   template<>
   struct result_traits< long double >
   {
      [[nodiscard]] static auto from( const char* value ) -> long double;
   };

}  // namespace tao::pq

#endif
