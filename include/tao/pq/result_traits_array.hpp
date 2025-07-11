// Copyright (c) 2021-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_TRAITS_ARRAY_HPP
#define TAO_PQ_RESULT_TRAITS_ARRAY_HPP

#include <stdexcept>
#include <string>

#include <tao/pq/is_array.hpp>
#include <tao/pq/result_traits.hpp>

namespace tao::pq
{
   namespace internal
   {
      template< typename T >
      concept array_result_type = pq::is_array_result< T > && ( pq::is_array_result< typename T::value_type > || ( result_traits_size< typename T::value_type > == 1 ) );

      [[nodiscard]] auto parse_quoted( const char*& value ) -> std::string;
      [[nodiscard]] auto parse_unquoted( const char*& value ) -> std::string;

      template< typename T >
      [[nodiscard]] auto parse( const char*& value ) -> T
      {
         if( *value == '"' ) {
            const std::string input = parse_quoted( ++value );
            return result_traits< T >::from( input.c_str() );
         }

         const std::string input = parse_unquoted( value );
         if( input == "NULL" ) {
            if constexpr( requires { result_traits< T >::null(); } ) {
               return result_traits< T >::null();
            }
            else {
               throw std::invalid_argument( "unexpected NULL value" );
            }
         }
         return result_traits< T >::from( input.c_str() );
      }

      template< typename T >
         requires pq::is_array_result< T >
      [[nodiscard]] auto parse( const char*& value ) -> T
      {
         if( *value++ != '{' ) {
            throw std::invalid_argument( "expected '{'" );
         }

         T container;
         if( *value == '}' ) {
            ++value;
            return container;
         }

         while( true ) {
            using value_type = typename T::value_type;
            if constexpr( requires { container.push_back( parse< value_type >( value ) ); } ) {
               container.push_back( parse< value_type >( value ) );
            }
            else {
               container.insert( parse< value_type >( value ) );
            }
            switch( *value++ ) {
               case ',':
               case ';':
                  break;

               case '}':
                  return container;

               default:
                  throw std::invalid_argument( "expected ',', ';', or '}'" );
            }
         }
      }

   }  // namespace internal

   template< internal::array_result_type T >
   struct result_traits< T >
   {
      static auto from( const char* value ) -> T
      {
         const auto container = internal::parse< T >( value );
         if( *value != '\0' ) {
            throw std::invalid_argument( "unexpected additional data" );
         }
         return container;
      }
   };

}  // namespace tao::pq

#endif
