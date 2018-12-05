// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_PQ_PARAMETER_TRAITS_HPP
#define TAO_PQ_PARAMETER_TRAITS_HPP

#include <cmath>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/pq/internal/printf.hpp>
#include <tao/pq/null.hpp>

namespace tao
{
   namespace pq
   {
      template< typename, typename = void >
      struct parameter_traits;

      namespace parameter_traits_impl
      {
         class char_pointer_helper
         {
         private:
            const char* p_;

         protected:
            explicit char_pointer_helper( const char* p ) noexcept
               : p_( p )
            {
            }

         public:
            [[nodiscard]] std::tuple< const char* > operator()() const
            {
               return std::tuple< const char* >( p_ );
            }
         };

         class string_helper
         {
         private:
            std::string s_;

         protected:
            template< typename... Ts >
            explicit string_helper( Ts&&... ts ) noexcept( noexcept( std::string( std::forward< Ts >( ts )... ) ) )
               : s_( std::forward< Ts >( ts )... )
            {
            }

         public:
            [[nodiscard]] std::tuple< const char* > operator()() const
            {
               return std::tuple< const char* >( s_.c_str() );
            }
         };

         template< typename T >
         [[nodiscard]] std::string printf_helper( const char* format, const T v )
         {
            if( std::isfinite( v ) ) {
               return internal::printf( format, v );
            }
            if( std::isnan( v ) ) {
               return "NAN";
            }
            return ( v < 0 ) ? "-INF" : "INF";
         }

      }  // namespace parameter_traits_impl

      template<>
      struct parameter_traits< null_t >
      {
         parameter_traits( const null_t& ) noexcept
         {
         }

         [[nodiscard]] std::tuple< const char* > operator()() const
         {
            return std::tuple< const char* >( nullptr );
         }
      };

      template<>
      struct parameter_traits< const char* >
         : parameter_traits_impl::char_pointer_helper
      {
         parameter_traits( const char* p ) noexcept
            : char_pointer_helper( p )
         {
         }
      };

      template<>
      struct parameter_traits< std::string >
         : parameter_traits_impl::char_pointer_helper
      {
         parameter_traits( const std::string& v ) noexcept
            : char_pointer_helper( v.c_str() )
         {
         }
      };

      template<>
      struct parameter_traits< bool >
         : parameter_traits_impl::char_pointer_helper
      {
         parameter_traits( const bool v ) noexcept
            : char_pointer_helper( v ? "TRUE" : "FALSE" )
         {
         }
      };

      template<>
      struct parameter_traits< char >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const char v )
            : string_helper( 1, v )
         {
         }
      };

      template<>
      struct parameter_traits< signed char >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const signed char v )
            : string_helper( internal::printf( "%hhd", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned char >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned char v )
            : string_helper( internal::printf( "%hhu", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< short >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const short v )
            : string_helper( internal::printf( "%hd", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned short >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned short v )
            : string_helper( internal::printf( "%hu", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< int >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const int v )
            : string_helper( internal::printf( "%d", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned v )
            : string_helper( internal::printf( "%u", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< long >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const long v )
            : string_helper( internal::printf( "%ld", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned long >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned long v )
            : string_helper( internal::printf( "%lu", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< long long >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const long long v )
            : string_helper( internal::printf( "%lld", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned long long >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned long long v )
            : string_helper( internal::printf( "%llu", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< float >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const float v )
            : string_helper( parameter_traits_impl::printf_helper( "%.9g", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< double >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const double v )
            : string_helper( parameter_traits_impl::printf_helper( "%.17g", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< long double >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const long double v )
            : string_helper( parameter_traits_impl::printf_helper( "%.21Lg", v ) )
         {
         }
      };

      template< typename T >
      struct parameter_traits< std::optional< T > >
      {
      private:
         std::optional< parameter_traits< std::decay_t< T > > > forwarder_;
         using result_type = decltype( ( *forwarder_ )() );

      public:
         parameter_traits( const std::optional< T >& v )
         {
            if( v ) {
               forwarder_.emplace( *v );
            }
         }

         parameter_traits( std::optional< T >&& v )
         {
            if( v ) {
               forwarder_.emplace( std::move( *v ) );
            }
         }

         [[nodiscard]] result_type operator()() const
         {
            return forwarder_ ? ( *forwarder_ )() : result_type();
         }
      };

   }  // namespace pq

}  // namespace tao

#endif
