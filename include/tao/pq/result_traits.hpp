// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_TRAITS_HPP
#define TAO_PQ_RESULT_TRAITS_HPP

#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <tao/pq/binary.hpp>
#include <tao/pq/bind.hpp>
#include <tao/pq/internal/exclusive_scan.hpp>
#include <tao/pq/is_aggregate.hpp>

namespace tao::pq
{
   template< typename >
   struct result_traits;

   template< typename >
   inline constexpr std::size_t result_traits_size = 1;

   template< typename T >
      requires requires { result_traits< T >::size; }
   inline constexpr std::size_t result_traits_size< T > = result_traits< T >::size;

   template< typename T >
   concept result_type_direct = ( ( result_traits_size< T > == 1 ) && !is_aggregate_result< T > ) && requires( const char* s ) {
      { result_traits< T >::from( s ) } -> std::same_as< T >;
   };

   class row;

   template< typename T >
   concept result_type_composite = ( ( result_traits_size< T > != 1 ) || is_aggregate_result< T > ) && requires( const row& r ) {
      { result_traits< T >::from( r ) } -> std::same_as< T >;
   };

   template< typename T >
   concept result_type = result_type_direct< T > || result_type_composite< T >;

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
      [[nodiscard]] static auto from( const char* value ) -> std::string_view
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

   template<>
   struct result_traits< std::string >
   {
      [[nodiscard]] static auto from( const char* value ) -> std::string
      {
         return value;
      }
   };

   template<>
   struct result_traits< binary >
   {
      [[nodiscard]] static auto from( const char* value ) -> binary;
   };

   namespace internal
   {
      template< typename, typename, typename >
      struct from_taopq;

      template< typename T, typename R, typename... As >
      struct from_taopq< T, R, R( As... ) >
      {
         static constexpr std::size_t size = ( 0 + ... + result_traits_size< std::decay_t< As > > );

         template< typename Row, std::size_t... Ns >
         [[nodiscard]] static auto from( const Row& row, std::index_sequence< Ns... > /*unused*/ ) -> R
         {
            return T::from_taopq( row.template get< std::decay_t< As > >( Ns )... );
         }

         template< typename Row >
         [[nodiscard]] static auto from( const Row& row ) -> R
         {
            return from_taopq::from( row, exclusive_scan_t< std::index_sequence< result_traits_size< std::decay_t< As > >... > >() );
         }
      };

      template< typename T, typename R, typename... As >
      struct from_taopq< T, R, R( As... ) noexcept >
         : from_taopq< T, R, R( As... ) >
      {};

   }  // namespace internal

   template< typename T >
      requires requires { T::from_taopq; }
   struct result_traits< T >
      : internal::from_taopq< T, T, decltype( T::from_taopq ) >
   {};

   template< typename T >
      requires requires { bind< T >::from_taopq; }
   struct result_traits< T >
      : internal::from_taopq< bind< T >, T, decltype( bind< T >::from_taopq ) >
   {};

}  // namespace tao::pq

#endif
