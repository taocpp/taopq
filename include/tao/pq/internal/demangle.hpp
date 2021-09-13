// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_DEMANGLE_HPP
#define TAO_PQ_INTERNAL_DEMANGLE_HPP

#include <ciso646>
#include <string_view>

namespace tao::pq::internal
{
#if defined( __clang__ )

   // #if defined( _LIBCPP_VERSION )

   template< typename T >
   [[nodiscard]] constexpr auto demangle() noexcept -> std::string_view
   {
      constexpr std::string_view sv = __PRETTY_FUNCTION__;
      constexpr auto begin = sv.find( '=' );
      static_assert( begin != std::string_view::npos );
      return sv.substr( begin + 2, sv.size() - begin - 3 );
   }

   // #else

   //    // When using libstdc++ with clang, std::string_view::find is not constexpr :(
   //    template< char C >
   //    constexpr auto find( const char* p, std::size_t n ) noexcept -> const char*
   //    {
   //       while( n ) {
   //          if( *p == C ) {
   //             return p;
   //          }
   //          ++p;
   //          --n;
   //       }
   //       return nullptr;
   //    }

   //    template< typename T >
   //    [[nodiscard]] constexpr auto demangle() noexcept -> std::string_view
   //    {
   //       constexpr std::string_view sv = __PRETTY_FUNCTION__;
   //       constexpr auto begin = find< '=' >( sv.data(), sv.size() );
   //       static_assert( begin != nullptr );
   //       return { begin + 2, sv.data() + sv.size() - begin - 3 };
   //    }

   // #endif

#elif defined( __GNUC__ )

#if( __GNUC__ == 9 ) && ( __GNUC_MINOR__ < 3 )

   // GCC 9.1 and 9.2 have a bug that leads to truncated __PRETTY_FUNCTION__ names,
   // see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=91155
   template< typename T >
   [[nodiscard]] constexpr auto demangle() noexcept -> std::string_view
   {
      // fallback: requires RTTI, no demangling
      return typeid( T ).name();
   }

#else

   template< typename T >
   [[nodiscard]] constexpr auto demangle() noexcept -> std::string_view
   {
      constexpr std::string_view sv = __PRETTY_FUNCTION__;
      constexpr auto begin = sv.find( '=' );
      static_assert( begin != std::string_view::npos );
      constexpr auto tmp = sv.substr( begin + 2 );
      constexpr auto end = tmp.rfind( ';' );
      static_assert( end != std::string_view::npos );
      return tmp.substr( 0, end );
   }

#endif

#elif defined( _MSC_VER )

#if( _MSC_VER < 1920 )

   template< typename T >
   [[nodiscard]] constexpr auto demangle() noexcept -> std::string_view
   {
      const std::string_view sv = __FUNCSIG__;
      const auto begin = sv.find( "demangle<" );
      const auto tmp = sv.substr( begin + 9 );
      const auto end = tmp.rfind( '>' );
      return tmp.substr( 0, end );
   }

#else

   template< typename T >
   [[nodiscard]] constexpr auto demangle() noexcept -> std::string_view
   {
      constexpr std::string_view sv = __FUNCSIG__;
      constexpr auto begin = sv.find( "demangle<" );
      static_assert( begin != std::string_view::npos );
      constexpr auto tmp = sv.substr( begin + 9 );
      constexpr auto end = tmp.rfind( '>' );
      static_assert( end != std::string_view::npos );
      return tmp.substr( 0, end );
   }

#endif

#else

   template< typename T >
   [[nodiscard]] constexpr auto demangle() noexcept -> std::string_view
   {
      // fallback: requires RTTI, no demangling
      return typeid( T ).name();
   }

#endif

}  // namespace tao::pq::internal

#endif
