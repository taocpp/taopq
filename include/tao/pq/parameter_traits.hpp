// Copyright (c) 2020-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_PARAMETER_TRAITS_HPP
#define TAO_PQ_PARAMETER_TRAITS_HPP

#include <cassert>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <tao/pq/binary.hpp>
#include <tao/pq/bind.hpp>
#include <tao/pq/internal/dependent_false.hpp>
#include <tao/pq/internal/parameter_traits_helper.hpp>
#include <tao/pq/internal/resize_uninitialized.hpp>
#include <tao/pq/null.hpp>
#include <tao/pq/oid.hpp>

namespace tao::pq
{
   namespace internal
   {
      // helper for arrays
      void array_append( std::string& buffer, std::string_view data );

      // helper for table_writer
      void table_writer_append( std::string& buffer, std::string_view data );

      template< std::size_t N, typename T >
      void snprintf( char ( &buffer )[ N ], const char* format, const T v ) noexcept
      {
         static_assert( N >= 32 );
         if( std::isfinite( v ) ) {
            [[maybe_unused]] const auto result = std::snprintf( buffer, N, format, v );
            assert( result > 0 );
            assert( static_cast< std::size_t >( result ) < N );
         }
         // clang-format off
         else if( std::isnan( v ) ) {
#if defined( _MSC_VER )
            [[maybe_unused]] const auto result = strncpy_s( buffer, "NAN", N );
            assert( result == 0 );
#else
            std::strcpy( buffer, "NAN" );  // NOLINT(clang-analyzer-security.insecureAPI.strcpy)
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
         // clang-format on
      }

   }  // namespace internal

   template< typename T, typename = void >
   struct parameter_traits
   {
      static_assert( internal::dependent_false< T >, "data type T not registered as taopq parameter type" );

      explicit parameter_traits( const T& /*unused*/ ) noexcept;

      static constexpr std::size_t columns = 1;
      static constexpr bool self_contained = true;

      template< std::size_t I >
      [[nodiscard]] static auto type() noexcept -> oid;

      template< std::size_t I >
      [[nodiscard]] static auto value() noexcept -> const char*;

      template< std::size_t I >
      [[nodiscard]] static auto length() noexcept -> int;

      template< std::size_t I >
      [[nodiscard]] static auto format() noexcept -> int;

      // for arrays
      template< std::size_t I >
      static void element( std::string& data );

      // for table_writer
      template< std::size_t I >
      static void copy_to( std::string& data );
   };

   template<>
   struct parameter_traits< null_t >
   {
      explicit parameter_traits( null_t /*unused*/ ) noexcept
      {}

      static constexpr std::size_t columns = 1;
      static constexpr bool self_contained = true;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return oid::invalid;
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
      static void element( std::string& data )
      {
         data += "NULL";
      }

      template< std::size_t I >
      static void copy_to( std::string& data )
      {
         data += "\\N";
      }
   };

   template<>
   struct parameter_traits< bool >
      : internal::char_pointer_helper
   {
      static constexpr bool self_contained = true;

      explicit parameter_traits( const bool v ) noexcept
         : internal::char_pointer_helper( v ? "TRUE" : "FALSE" )
      {}

      template< std::size_t I >
      void element( std::string& data ) const
      {
         data += m_p;
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         data += m_p;
      }
   };

   template<>
   struct parameter_traits< char >
   {
      const char m_value[ 2 ];

      explicit parameter_traits( const char v ) noexcept
         : m_value{ v, '\0' }
      {}

      static constexpr std::size_t columns = 1;
      static constexpr bool self_contained = true;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return oid::invalid;
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
      void element( std::string& data ) const
      {
         internal::array_append( data, std::string_view( m_value, 1 ) );
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         internal::table_writer_append( data, std::string_view( m_value, 1 ) );
      }
   };

   template<>
   struct parameter_traits< signed char >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< unsigned char >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< short >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< unsigned short >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< int >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< unsigned int >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< long >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< unsigned long >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< long long >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< unsigned long long >
      : internal::to_chars_helper
   {
      using internal::to_chars_helper::to_chars_helper;
   };

   template<>
   struct parameter_traits< float >
      : internal::buffer_helper
   {
      explicit parameter_traits( const float v ) noexcept
      {
         internal::snprintf( m_buffer, "%.9g", v );
      }
   };

   template<>
   struct parameter_traits< double >
      : internal::buffer_helper
   {
      explicit parameter_traits( const double v ) noexcept
      {
         internal::snprintf( m_buffer, "%.17g", v );
      }
   };

   template<>
   struct parameter_traits< long double >
      : internal::buffer_helper
   {
      explicit parameter_traits( const long double v ) noexcept
      {
         internal::snprintf( m_buffer, "%.21Lg", v );
      }
   };

   template<>
   struct parameter_traits< const char* >
      : internal::char_pointer_helper
   {
      using internal::char_pointer_helper::char_pointer_helper;

      template< std::size_t I >
      void element( std::string& data ) const
      {
         internal::array_append( data, m_p );
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         internal::table_writer_append( data, m_p );
      }
   };

   // for string_views (which are not zero-terminated) we can use binary format and,
   // surprisingly, it does not seem to cause any issues

   template<>
   struct parameter_traits< std::string_view >
   {
   private:
      const std::string_view m_v;

   public:
      explicit parameter_traits( const std::string_view v ) noexcept
         : m_v( v )
      {}

      static constexpr std::size_t columns = 1;
      static constexpr bool self_contained = false;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return oid::text;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return m_v.data();
      }

      template< std::size_t I >
      [[nodiscard]] auto length() const noexcept -> int
      {
         return static_cast< int >( m_v.size() );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      void element( std::string& data ) const
      {
         internal::array_append( data, m_v );
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         internal::table_writer_append( data, m_v );
      }
   };

   template<>
   struct parameter_traits< std::string >
      : parameter_traits< std::string_view >
   {
      using parameter_traits< std::string_view >::parameter_traits;
   };

   template< std::size_t Extent >
   struct parameter_traits< std::span< const std::byte, Extent > >
   {
   private:
      const std::span< const std::byte, Extent > m_v;

   public:
      explicit parameter_traits( const std::span< const std::byte, Extent > v ) noexcept
         : m_v( v )
      {}

      static constexpr std::size_t columns = 1;
      static constexpr bool self_contained = false;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return oid::bytea;
      }

      template< std::size_t I >
      [[nodiscard]] constexpr auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( m_v.data() );
      }

      template< std::size_t I >
      [[nodiscard]] constexpr auto length() const noexcept -> int
      {
         return static_cast< int >( m_v.size() );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      void element( std::string& data ) const
      {
         // generate bytea hex format
         constexpr char hex[] = "0123456789abcdef";
         auto pos = data.size();
         internal::resize_uninitialized( data, pos + 3 + 2 * m_v.size() );
         data[ pos++ ] = '\\';
         data[ pos++ ] = '\\';
         data[ pos++ ] = 'x';
         for( auto c : m_v ) {
            data[ pos++ ] = hex[ static_cast< unsigned char >( c ) >> 4 ];
            data[ pos++ ] = hex[ static_cast< unsigned char >( c ) & 15 ];
         }
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         element< I >( data );
      }
   };

   template< std::size_t Extent >
   struct parameter_traits< std::span< std::byte, Extent > >
      : parameter_traits< std::span< const std::byte, Extent > >
   {
      using parameter_traits< std::span< const std::byte, Extent > >::parameter_traits;
   };

   template< typename Allocator >
   struct parameter_traits< std::vector< std::byte, Allocator > >
      : parameter_traits< binary_view >
   {
      using parameter_traits< binary_view >::parameter_traits;
   };

   // default free function to detect member function to_taopq()
   template< typename T >
   [[nodiscard]] auto to_taopq( const T& t ) noexcept( noexcept( t.to_taopq() ) )
      -> decltype( t.to_taopq() )
   {
      return t.to_taopq();
   }

   // default free function to detect bind<T>::to_taopq()
   template< typename T >
   [[nodiscard]] auto to_taopq( const T& t ) noexcept( noexcept( bind< T >::to_taopq( t ) ) )
      -> decltype( bind< T >::to_taopq( t ) )
   {
      return bind< T >::to_taopq( t );
   }

   // note: calls to to_taopq are unqualified to enable ADL

   namespace internal
   {
      template< typename T >
      struct parameter_holder
      {
         using result_t = decltype( to_taopq( std::declval< const T& >() ) );
         const result_t result;

         explicit parameter_holder( const T& t ) noexcept( noexcept( result_t( to_taopq( t ) ) ) )
            : result( to_taopq( t ) )
         {}
      };

   }  // namespace internal

   template< typename T >
   struct parameter_traits< T, decltype( (void)to_taopq( std::declval< const T& >() ) ) >
      : private internal::parameter_holder< T >,
        public parameter_traits< typename internal::parameter_holder< T >::result_t >
   {
      static constexpr bool self_contained = true;

      using typename internal::parameter_holder< T >::result_t;

      explicit parameter_traits( const T& t ) noexcept( noexcept( internal::parameter_holder< T >( t ),
                                                                  parameter_traits< result_t >( std::declval< result_t >() ) ) )
         : internal::parameter_holder< T >( t ),
           parameter_traits< result_t >( this->result )
      {}
   };

}  // namespace tao::pq

#endif
