// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_PARAMETER_TRAITS_HPP
#define TAO_PQ_PARAMETER_TRAITS_HPP

#include <cassert>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <utility>

#include <tao/pq/binary.hpp>
#include <tao/pq/fwd.hpp>
#include <tao/pq/internal/dependent_false.hpp>
#include <tao/pq/internal/endian.hpp>
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

   }  // namespace internal

   template< typename T, typename >
   struct parameter_traits
   {
      static_assert( internal::dependent_false< T >, "data type T not registered as taopq parameter" );

      explicit parameter_traits( const T& /*unused*/ ) noexcept;

      static constexpr std::size_t columns = 1;

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

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
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
   {
      const bool m_v;

      explicit parameter_traits( const bool v ) noexcept
         : m_v( v )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 16;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      void element( std::string& data ) const
      {
         data += ( m_v ? "TRUE" : "FALSE" );
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         data += ( m_v ? "TRUE" : "FALSE" );
      }
   };

   template<>
   struct parameter_traits< char >
   {
      const char m_v;

      explicit parameter_traits( const char v ) noexcept
         : m_v( v )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 18;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }
   };

   template<>
   struct parameter_traits< short >
   {
      const short m_v;

      static_assert( sizeof( short ) == 2 );

      explicit parameter_traits( const short v ) noexcept
         : m_v( internal::hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 21;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( short );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      void element( std::string& data ) const
      {
         char buffer[ 32 ];
         const auto [ ptr, ec ] = std::to_chars( std::begin( buffer ), std::end( buffer ), internal::hton( m_v ) );
         assert( ec == std::errc() );
         data.append( buffer, ptr );
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         element< I >( data );
      }
   };

   template<>
   struct parameter_traits< int >
   {
      const int m_v;

      static_assert( sizeof( int ) == 4 );

      explicit parameter_traits( const int v ) noexcept
         : m_v( internal::hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 23;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( int );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      void element( std::string& data ) const
      {
         char buffer[ 32 ];
         const auto [ ptr, ec ] = std::to_chars( std::begin( buffer ), std::end( buffer ), internal::hton( m_v ) );
         assert( ec == std::errc() );
         data.append( buffer, ptr );
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         element< I >( data );
      }
   };

   template<>
   struct parameter_traits< long >
   {
      const long m_v;

      static_assert( ( sizeof( long ) == 4 ) || ( sizeof( long ) == 8 ) );

      explicit parameter_traits( const long v ) noexcept
         : m_v( internal::hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return ( sizeof( long ) == 4 ) ? 23 : 20;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( long );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      void element( std::string& data ) const
      {
         char buffer[ 32 ];
         const auto [ ptr, ec ] = std::to_chars( std::begin( buffer ), std::end( buffer ), internal::hton( m_v ) );
         assert( ec == std::errc() );
         data.append( buffer, ptr );
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         element< I >( data );
      }
   };

   template<>
   struct parameter_traits< long long >
   {
      const long long m_v;

      static_assert( sizeof( long long ) == 8 );

      explicit parameter_traits( const long long v ) noexcept
         : m_v( internal::hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 20;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( long long );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      void element( std::string& data ) const
      {
         char buffer[ 32 ];
         const auto [ ptr, ec ] = std::to_chars( std::begin( buffer ), std::end( buffer ), internal::hton( m_v ) );
         assert( ec == std::errc() );
         data.append( buffer, ptr );
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         element< I >( data );
      }
   };

   template<>
   struct parameter_traits< unsigned long long >
      : internal::to_chars_helper
   {
      parameter_traits( const unsigned long long v )
         : internal::to_chars_helper( v )
      {}
   };

   template<>
   struct parameter_traits< signed char >
      : parameter_traits< short >
   {
      using parameter_traits< short >::parameter_traits;
   };

   template<>
   struct parameter_traits< unsigned char >
      : parameter_traits< short >
   {
      explicit parameter_traits( const unsigned char v )
         : parameter_traits< short >( static_cast< short >( v ) )
      {}
   };

   template< typename T >
   struct parameter_traits< T, std::enable_if_t< std::is_same_v< T, unsigned short > && ( sizeof( unsigned short ) < sizeof( long long ) ) > >
      : parameter_traits< long long >
   {
      explicit parameter_traits( const unsigned short v )
         : parameter_traits< long long >( static_cast< long long >( v ) )
      {}
   };

   template< typename T >
   struct parameter_traits< T, std::enable_if_t< std::is_same_v< T, unsigned short > && !( sizeof( unsigned short ) < sizeof( long long ) ) > >
      : parameter_traits< unsigned long long >
   {
      explicit parameter_traits( const unsigned short v )
         : parameter_traits< unsigned long long >( static_cast< unsigned long long >( v ) )
      {}
   };

   template< typename T >
   struct parameter_traits< T, std::enable_if_t< std::is_same_v< T, unsigned > && ( sizeof( unsigned ) < sizeof( long long ) ) > >
      : parameter_traits< long long >
   {
      explicit parameter_traits( const unsigned v )
         : parameter_traits< long long >( static_cast< long long >( v ) )
      {}
   };

   template< typename T >
   struct parameter_traits< T, std::enable_if_t< std::is_same_v< T, unsigned > && !( sizeof( unsigned ) < sizeof( long long ) ) > >
      : parameter_traits< unsigned long long >
   {
      explicit parameter_traits( const unsigned v )
         : parameter_traits< unsigned long long >( static_cast< unsigned long long >( v ) )
      {}
   };

   template< typename T >
   struct parameter_traits< T, std::enable_if_t< std::is_same_v< T, unsigned long > && ( sizeof( unsigned long ) < sizeof( long long ) ) > >
      : parameter_traits< long long >
   {
      explicit parameter_traits( const unsigned long v )
         : parameter_traits< long long >( static_cast< long long >( v ) )
      {}
   };

   template< typename T >
   struct parameter_traits< T, std::enable_if_t< std::is_same_v< T, unsigned long > && !( sizeof( unsigned long ) < sizeof( long long ) ) > >
      : parameter_traits< unsigned long long >
   {
      explicit parameter_traits( const unsigned long v )
         : parameter_traits< unsigned long long >( static_cast< unsigned long long >( v ) )
      {}
   };

   template<>
   struct parameter_traits< float >
   {
      const float m_v;

      static_assert( sizeof( float ) == 4 );

      explicit parameter_traits( const float v ) noexcept
         : m_v( internal::hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 700;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( float );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      void element( std::string& data ) const
      {
         char buffer[ 32 ];
         internal::snprintf( buffer, "%.9g", internal::hton( m_v ) );
         data += buffer;
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         element< I >( data );
      }
   };

   template<>
   struct parameter_traits< double >
   {
      const double m_v;

      static_assert( sizeof( double ) == 8 );

      explicit parameter_traits( const double v ) noexcept
         : m_v( internal::hton( v ) )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 701;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return reinterpret_cast< const char* >( &m_v );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return sizeof( double );
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 1;
      }

      template< std::size_t I >
      void element( std::string& data ) const
      {
         char buffer[ 32 ];
         internal::snprintf( buffer, "%.17g", internal::hton( m_v ) );
         data += buffer;
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         element< I >( data );
      }
   };

   template<>
   struct parameter_traits< long double >
      : internal::buffer_helper
   {
      parameter_traits( const long double v )
      {
         internal::snprintf( m_buffer, "%.21Lg", v );
      }
   };

   template<>
   struct parameter_traits< const char* >
      : internal::char_pointer_helper
   {
      explicit parameter_traits( const char* p ) noexcept
         : internal::char_pointer_helper( p )
      {}

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

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 25;
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

   template<>
   struct parameter_traits< std::basic_string_view< unsigned char > >
   {
   private:
      const std::basic_string_view< unsigned char > m_v;

   public:
      explicit parameter_traits( const std::basic_string_view< unsigned char > v ) noexcept
         : m_v( v )
      {}

      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return 17;
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
         static char hex[] = "0123456789ABCDEF";
         auto pos = data.size();
         internal::resize_uninitialized( data, pos + 2 + m_v.size() * 2 );
         data[ pos++ ] = '\\';
         data[ pos++ ] = 'x';
         for( auto c : m_v ) {
            data[ pos++ ] = hex[ ( c >> 4 ) % 15 ];
            data[ pos++ ] = hex[ c % 15 ];
         }
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         element< I >( data );
      }
   };

   template<>
   struct parameter_traits< std::basic_string< unsigned char > >
      : parameter_traits< std::basic_string_view< unsigned char > >
   {
      using parameter_traits< std::basic_string_view< unsigned char > >::parameter_traits;
   };

   template<>
   struct parameter_traits< binary_view >
      : parameter_traits< std::basic_string_view< unsigned char > >
   {
      parameter_traits( const binary_view v )
         : parameter_traits< std::basic_string_view< unsigned char > >( std::basic_string_view< unsigned char >( reinterpret_cast< const unsigned char* >( v.data() ), v.size() ) )
      {}
   };

   template<>
   struct parameter_traits< binary >
      : parameter_traits< binary_view >
   {
      using parameter_traits< binary_view >::parameter_traits;
   };

   // default free function to detect member function to_taopq_param()
   template< typename T >
   auto to_taopq_param( const T& t ) noexcept( noexcept( t.to_taopq_param() ) )
      -> decltype( t.to_taopq_param() )
   {
      return t.to_taopq_param();
   }

   namespace internal
   {
      // note: calls to to_taopq_param are unqualified to enable ADL
      template< typename T >
      struct parameter_holder
      {
         using result_t = decltype( to_taopq_param( std::declval< const T& >() ) );
         const result_t result;

         explicit parameter_holder( const T& t ) noexcept( noexcept( result_t( to_taopq_param( t ) ) ) )
            : result( to_taopq_param( t ) )
         {}
      };

   }  // namespace internal

   template< typename T >
   struct parameter_traits< T, std::void_t< decltype( to_taopq_param( std::declval< const T& >() ) ) > >
      : private internal::parameter_holder< T >,
        public parameter_traits< typename internal::parameter_holder< T >::result_t >
   {
      using typename internal::parameter_holder< T >::result_t;

      explicit parameter_traits( const T& t ) noexcept( noexcept( internal::parameter_holder< T >( t ),
                                                                  parameter_traits< result_t >( std::declval< result_t >() ) ) )
         : internal::parameter_holder< T >( t ),
           parameter_traits< result_t >( this->result )
      {}
   };

}  // namespace tao::pq

#endif
