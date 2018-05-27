// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_PARAMETER_TRAITS_HPP
#define TAO_POSTGRES_PARAMETER_TRAITS_HPP

#include <cmath>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <tao/optional/optional.hpp>
#include <tao/postgres/null.hpp>
#include <tao/seq/make_integer_sequence.hpp>
#include <tao/utility/printf.hpp>

namespace tao
{
   namespace postgres
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
            explicit char_pointer_helper( const char* p )
               : p_( p )
            {
            }

         public:
            std::tuple< const char* > operator()() const
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
            explicit string_helper( Ts&&... ts )
               : s_( std::forward< Ts >( ts )... )
            {
            }

         public:
            std::tuple< const char* > operator()() const
            {
               return std::tuple< const char* >( s_.c_str() );
            }
         };

         template< typename T >
         std::string printf_helper( const char* format, const T v )
         {
            if( std::isfinite( v ) ) {
               return utility::printf( format, v );
            }
            if( std::isnan( v ) ) {
               return "NAN";
            }
            return ( v < 0 ) ? "-INF" : "INF";
         }

         template< typename... Ts >
         class decay_helper
         {
         private:
            std::tuple< parameter_traits< typename std::decay< Ts >::type >... > value_;

            template< std::size_t... Ns >
            auto impl( const seq::index_sequence< Ns... >& ) const -> decltype( std::tuple_cat( std::get< Ns >( value_ )()... ) )
            {
               return std::tuple_cat( std::get< Ns >( value_ )()... );
            }

         protected:
            template< typename... Us >
            explicit decay_helper( Us&&... us )
               : value_( std::forward< Us >( us )... )
            {
            }

         public:
            auto operator()() const -> decltype( impl( seq::index_sequence_for< Ts... >() ) )
            {
               return impl( seq::index_sequence_for< Ts... >() );
            }
         };

      }  // namespace parameter_traits_impl

      template<>
      struct parameter_traits< null_t >
      {
         parameter_traits( const null_t& )
         {
         }

         std::tuple< const char* > operator()() const
         {
            return std::tuple< const char* >( nullptr );
         }
      };

      template<>
      struct parameter_traits< const char* >
         : parameter_traits_impl::char_pointer_helper
      {
         parameter_traits( const char* p )
            : char_pointer_helper( p )
         {
         }
      };

      template<>
      struct parameter_traits< std::string >
         : parameter_traits_impl::char_pointer_helper
      {
         parameter_traits( const std::string& v )
            : char_pointer_helper( v.c_str() )
         {
         }
      };

      template<>
      struct parameter_traits< bool >
         : parameter_traits_impl::char_pointer_helper
      {
         parameter_traits( const bool v )
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
            : string_helper( utility::printf( "%hhd", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned char >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned char v )
            : string_helper( utility::printf( "%hhu", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< short >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const short v )
            : string_helper( utility::printf( "%hd", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned short >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned short v )
            : string_helper( utility::printf( "%hu", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< int >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const int v )
            : string_helper( utility::printf( "%d", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned v )
            : string_helper( utility::printf( "%u", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< long >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const long v )
            : string_helper( utility::printf( "%ld", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned long >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned long v )
            : string_helper( utility::printf( "%lu", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< long long >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const long long v )
            : string_helper( utility::printf( "%lld", v ) )
         {
         }
      };

      template<>
      struct parameter_traits< unsigned long long >
         : parameter_traits_impl::string_helper
      {
         parameter_traits( const unsigned long long v )
            : string_helper( utility::printf( "%llu", v ) )
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
      struct parameter_traits< optional< T > >
      {
      private:
         optional< parameter_traits< typename std::decay< T >::type > > forwarder_;
         using result_type = decltype( ( *forwarder_ )() );

      public:
         parameter_traits( const optional< T >& v )
         {
            if( v ) {
               forwarder_.emplace( *v );
            }
         }

         result_type operator()() const
         {
            return forwarder_ ? ( *forwarder_ )() : result_type();
         }
      };

   }  // namespace postgres

}  // namespace tao

#endif
