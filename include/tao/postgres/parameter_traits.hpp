// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_POSTGRES_PARAMETER_TRAITS_HPP
#define TAOCPP_INCLUDE_POSTGRES_PARAMETER_TRAITS_HPP

#include <tuple>
#include <string>
#include <type_traits>
#include <utility>

#include <tao/seq/make_integer_sequence.hpp>
#include <tao/optional/optional.hpp>
#include <tao/utility/printf.hpp>
#include <tao/postgres/null.hpp>

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
    }

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

    template< typename T >
    struct parameter_traits< T, typename std::enable_if< std::is_arithmetic< T >::value >::type >
      : parameter_traits_impl::string_helper
    {
      static const char* const format;

      parameter_traits( const T v )
        : string_helper( utility::printf( format, v ) )
      {
      }
    };

    template< typename T >
    struct parameter_traits< optional< T > >
    {
    private:
      optional< parameter_traits< typename std::decay< T >::type > > forwarder_;
      using result_type = decltype( (*forwarder_)() );

    public:
      parameter_traits( const optional< T >& v )
      {
        if( v ) {
          forwarder_.emplace( *v );
        }
      }

      result_type operator()() const
      {
        return forwarder_ ? (*forwarder_)() : result_type();
      }
    };
  }
}

#endif // TAOCPP_INCLUDE_POSTGRES_PARAMETER_TRAITS_HPP
