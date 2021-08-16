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

#include <array>
#include <list>
#include <set>
#include <unordered_set>
#include <vector>

#include <tao/pq/binary.hpp>
#include <tao/pq/internal/dependent_false.hpp>
#include <tao/pq/internal/parameter_traits_helper.hpp>
#include <tao/pq/internal/to_traits.hpp>

#include <libpq-fe.h>

namespace tao::pq::internal
{
   template< std::size_t N, typename T >
   void snprintf( char ( &buffer )[ N ], const char* format, const T v )
   {
      static_assert( N >= 32 );
      if( std::isfinite( v ) ) {
         [[maybe_unused]] const auto result = std::snprintf( buffer, N, format, v );
         assert( result > 0 );
         assert( static_cast< std::size_t >( result ) < N );
      }
      else if( std::isnan( v ) ) {
#if defined( _MSC_VER )
         [[maybe_unused]] const auto result = strncpy_s( buffer, "NAN", N );
         assert( result == 0 );
#else
         std::strcpy( buffer, "NAN" );                       // NOLINT(clang-analyzer-security.insecureAPI.strcpy)
#endif
      }
      else {
#if defined( _MSC_VER )
         [[maybe_unused]] const auto result = strncpy_s( buffer, ( v < 0 ) ? "-INF" : "INF", N );
         assert( result == 0 );
#else
         std::strcpy( buffer, ( v < 0 ) ? "-INF" : "INF" );  // NOLINT(clang-analyzer-security.insecureAPI.strcpy)
#endif
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
   {
      const char m_value[ 2 ];

      parameter_text_traits( const char v )
         : m_value{ v, '\0' }
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return m_value;
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
         return std::string_view( m_value, 1 );
      }
   };

   template<>
   struct parameter_text_traits< signed char >
      : to_chars_helper
   {
      parameter_text_traits( const signed char v )
         : to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned char >
      : to_chars_helper
   {
      parameter_text_traits( const unsigned char v )
         : to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_text_traits< short >
      : to_chars_helper
   {
      parameter_text_traits( const short v )
         : to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned short >
      : to_chars_helper
   {
      parameter_text_traits( const unsigned short v )
         : to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_text_traits< int >
      : to_chars_helper
   {
      parameter_text_traits( const int v )
         : to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned >
      : to_chars_helper
   {
      parameter_text_traits( const unsigned v )
         : to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_text_traits< long >
      : to_chars_helper
   {
      parameter_text_traits( const long v )
         : to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned long >
      : to_chars_helper
   {
      parameter_text_traits( const unsigned long v )
         : to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_text_traits< long long >
      : to_chars_helper
   {
      parameter_text_traits( const long long v )
         : to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_text_traits< unsigned long long >
      : to_chars_helper
   {
      parameter_text_traits( const unsigned long long v )
         : to_chars_helper( v )
      {}
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
   struct parameter_text_traits< std::basic_string_view< unsigned char > >
   {
   private:
      unsigned char* m_data;

   public:
      parameter_text_traits( PGconn* c, const std::basic_string_view< unsigned char > v, std::size_t dummy = 0 )
         : m_data( PQescapeByteaConn( c, v.data(), v.size(), &dummy ) )  // NOLINT
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

   template<>
   struct parameter_text_traits< binary_view >
      : parameter_text_traits< std::basic_string_view< unsigned char > >
   {
      parameter_text_traits( PGconn* c, const binary_view v )
         : parameter_text_traits< std::basic_string_view< unsigned char > >( c, std::basic_string_view< unsigned char >( reinterpret_cast< const unsigned char* >( v.data() ), v.size() ) )
      {}
   };

   template< typename >
   inline constexpr const bool is_array_parameter = false;

   template< typename T, std::size_t N >
   inline constexpr const bool is_array_parameter< std::array< T, N > > = true;

   template< typename... Ts >
   inline constexpr const bool is_array_parameter< std::list< Ts... > > = true;

   template< typename... Ts >
   inline constexpr const bool is_array_parameter< std::set< Ts... > > = true;

   template< typename... Ts >
   inline constexpr const bool is_array_parameter< std::unordered_set< Ts... > > = true;

   template< typename... Ts >
   inline constexpr const bool is_array_parameter< std::vector< Ts... > > = true;

   inline std::string array_escape( std::string_view data )
   {
      std::string nrv;
      nrv += '"';
      while( true ) {
         const auto n = data.find_first_of( "\\\"" );
         if( n == std::string_view::npos ) {
            nrv += data;
            nrv += '"';
            return nrv;
         }
         nrv.append( data.data(), n );
         nrv += '\\';
         nrv += data[ n ];
         data.remove_prefix( n + 1 );
      }
   }

   template< template< typename... > class Traits, typename T >
   auto to_array( PGconn* c, const T& v )
      -> std::enable_if_t< !is_array_parameter< T >, std::string >
   {
      const auto t = internal::to_traits< Traits >( c, v );
      static_assert( t.columns == 1 );
      const char* s = t.template value< 0 >();
      if( s == nullptr ) {
         return "NULL";
      }
      if( s[ 0 ] == '\0' ) {
         return "\"\"";
      }
      if( s == std::string_view( "NULL" ) ) {
         return "\"NULL\"";
      }
      if( const auto* pos = std::strpbrk( s, "\\\"" ) ) {
         return array_escape( s );
      }
      if( const auto* pos = std::strpbrk( s, "{},; \t" ) ) {
         return '"' + std::string( s ) + '"';
      }
      return s;
   }

   template< template< typename... > class Traits, typename T >
   auto to_array( PGconn* c, const T& v )
      -> std::enable_if_t< is_array_parameter< T >, std::string >
   {
      std::string nrv;
      nrv += '{';
      if( v.empty() ) {
         nrv += '}';
         return nrv;
      }
      for( const auto& e : v ) {
         nrv += internal::to_array< Traits >( c, e );
         nrv += ',';
      }
      nrv[ nrv.size() - 1 ] = '}';
      return nrv;
   }

   template< typename T >
   struct parameter_text_traits< T, std::enable_if_t< is_array_parameter< T > > >
   {
   private:
      const std::string m_data;

   public:
      template< template< typename... > class Traits >
      parameter_text_traits( PGconn* c, const T& v, wrap_traits< Traits > /*unused*/ )
         : m_data( internal::to_array< Traits >( c, v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return m_data.c_str();
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
         return m_data;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto escape() noexcept -> bool
      {
         return false;
      }
   };

}  // namespace tao::pq::internal

#endif
