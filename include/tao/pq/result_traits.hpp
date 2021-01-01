// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_RESULT_TRAITS_HPP
#define TAO_PQ_RESULT_TRAITS_HPP

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/dependent_false.hpp>

namespace tao::pq
{
   class row;

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

   template<>
   struct result_traits< const char* >
   {
      [[nodiscard]] static auto from( const char* value ) -> const char*
      {
         return value;
      }
   };

   template<>
   struct result_traits< std::string >
   {
      [[nodiscard]] static auto from( const char* value ) -> std::string
      {
         return value;
      }
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
      [[nodiscard]] static auto from( const char* value ) -> signed char;
   };

   template<>
   struct result_traits< unsigned char >
   {
      [[nodiscard]] static auto from( const char* value ) -> unsigned char;
   };

   template<>
   struct result_traits< short >
   {
      [[nodiscard]] static auto from( const char* value ) -> short;
   };

   template<>
   struct result_traits< unsigned short >
   {
      [[nodiscard]] static auto from( const char* value ) -> unsigned short;
   };

   template<>
   struct result_traits< int >
   {
      [[nodiscard]] static auto from( const char* value ) -> int;
   };

   template<>
   struct result_traits< unsigned >
   {
      [[nodiscard]] static auto from( const char* value ) -> unsigned;
   };

   template<>
   struct result_traits< long >
   {
      [[nodiscard]] static auto from( const char* value ) -> long;
   };

   template<>
   struct result_traits< unsigned long >
   {
      [[nodiscard]] static auto from( const char* value ) -> unsigned long;
   };

   template<>
   struct result_traits< long long >
   {
      [[nodiscard]] static auto from( const char* value ) -> long long;
   };

   template<>
   struct result_traits< unsigned long long >
   {
      [[nodiscard]] static auto from( const char* value ) -> unsigned long long;
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
