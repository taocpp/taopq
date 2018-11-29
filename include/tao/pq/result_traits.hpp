// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_PQ_RESULT_TRAITS_HPP
#define TAO_PQ_RESULT_TRAITS_HPP

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace tao
{
   namespace pq
   {
      class row;

      template< typename T, typename = void >
      struct result_traits
      {
         static_assert( !std::is_same< T, T >::value, "tao::pq::result_traits<T> not specialized for T" );
         static T from( const char* value );
      };

      template< typename T, typename = void >
      inline constexpr std::size_t result_traits_size = 1;

      template< typename T >
      inline constexpr std::size_t result_traits_size< T, std::enable_if_t< std::is_same_v< decltype( result_traits< T >::size ), const std::size_t > > > = result_traits< T >::size;

      template< typename T, typename = void >
      inline constexpr bool result_traits_has_null = false;

      template< typename T >
      inline constexpr bool result_traits_has_null< T, decltype( (void)result_traits< T >::null() ) > = true;

      template<>
      struct result_traits< const char* >
      {
         [[nodiscard]] static const char* from( const char* value )
         {
            return value;
         }
      };

      template<>
      struct result_traits< std::string >
      {
         [[nodiscard]] static std::string from( const char* value )
         {
            return value;
         }
      };

      template<>
      struct result_traits< bool >
      {
         [[nodiscard]] static bool from( const char* value );
      };

      template<>
      struct result_traits< char >
      {
         [[nodiscard]] static char from( const char* value );
      };

      template<>
      struct result_traits< signed char >
      {
         [[nodiscard]] static signed char from( const char* value );
      };

      template<>
      struct result_traits< unsigned char >
      {
         [[nodiscard]] static unsigned char from( const char* value );
      };

      template<>
      struct result_traits< short >
      {
         [[nodiscard]] static short from( const char* value );
      };

      template<>
      struct result_traits< unsigned short >
      {
         [[nodiscard]] static unsigned short from( const char* value );
      };

      template<>
      struct result_traits< int >
      {
         [[nodiscard]] static int from( const char* value );
      };

      template<>
      struct result_traits< unsigned >
      {
         [[nodiscard]] static unsigned from( const char* value );
      };

      template<>
      struct result_traits< long >
      {
         [[nodiscard]] static long from( const char* value );
      };

      template<>
      struct result_traits< unsigned long >
      {
         [[nodiscard]] static unsigned long from( const char* value );
      };

      template<>
      struct result_traits< long long >
      {
         [[nodiscard]] static long long from( const char* value );
      };

      template<>
      struct result_traits< unsigned long long >
      {
         [[nodiscard]] static unsigned long long from( const char* value );
      };

      template<>
      struct result_traits< float >
      {
         [[nodiscard]] static float from( const char* value );
      };

      template<>
      struct result_traits< double >
      {
         [[nodiscard]] static double from( const char* value );
      };

      template<>
      struct result_traits< long double >
      {
         [[nodiscard]] static long double from( const char* value );
      };

   }  // namespace pq

}  // namespace tao

#endif
