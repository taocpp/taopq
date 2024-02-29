// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_TRAITS_ARRAY_HPP
#define TAO_PQ_RESULT_TRAITS_ARRAY_HPP

#include <array>
#include <cstring>
#include <list>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <tao/pq/result_traits.hpp>

namespace tao::pq
{
   template< typename >
   inline constexpr bool is_array_result = false;

   template< typename... Ts >
   inline constexpr bool is_array_result< std::list< Ts... > > = true;

   template< typename... Ts >
   inline constexpr bool is_array_result< std::set< Ts... > > = true;

   template< typename... Ts >
   inline constexpr bool is_array_result< std::unordered_set< Ts... > > = true;

   template< typename... Ts >
   inline constexpr bool is_array_result< std::vector< Ts... > > = true;

   namespace internal
   {
      template< typename T >
      void parse_elements( T& container, const char*& value );

      template< typename T >
      void parse_element( T& container, const char*& value )
      {
         using value_type = typename T::value_type;
         if constexpr( is_array_result< value_type > ) {
            value_type element;
            internal::parse_elements( element, value );
            container.push_back( std::move( element ) );
         }
         else {
            if( *value == '"' ) {
               ++value;
               std::string input;
               while( const auto* pos = std::strpbrk( value, "\\\"" ) ) {
                  if( *pos == '\\' ) {
                     input.append( value, pos++ );
                     input += *pos++;
                     value = pos;
                  }
                  else {
                     input.append( value, pos++ );
                     value = pos;
                     break;
                  }
               }
               container.push_back( result_traits< value_type >::from( input.c_str() ) );
            }
            else if( *value != '}' ) {
               if( const auto* end = std::strpbrk( value, ",;}" ) ) {
                  std::string input( value, end );
                  if( input == "NULL" ) {
                     if constexpr( result_traits_has_null< value_type > ) {
                        container.push_back( result_traits< value_type >::null() );
                     }
                     else {
                        throw std::invalid_argument( "unexpected NULL value" );
                     }
                  }
                  else {
                     container.push_back( result_traits< value_type >::from( input.c_str() ) );
                  }
                  value = end;
               }
               else {
                  throw std::invalid_argument( "unterminated unquoted string" );
               }
            }
         }
      }

      template< typename T >
      void parse_elements( T& container, const char*& value )
      {
         if( *value++ != '{' ) {
            throw std::invalid_argument( "expected '{'" );
         }
         while( true ) {
            internal::parse_element( container, value );
            switch( *value++ ) {
               case ',':
               case ';':
                  continue;
               case '}':
                  return;
               default:
                  throw std::invalid_argument( "expected ',', ';', or '}'" );
            }
         }
      }

   }  // namespace internal

   template< typename T >
   struct result_traits< T, std::enable_if_t< is_array_result< T > > >
   {
      static auto from( const char* value ) -> T
      {
         T nrv;
         internal::parse_elements( nrv, value );
         if( *value != '\0' ) {
            throw std::invalid_argument( "unexpected additional data" );
         }
         return nrv;
      }
   };

}  // namespace tao::pq

#endif
