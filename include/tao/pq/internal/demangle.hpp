// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_DEMANGLE_HPP
#define TAO_PQ_INTERNAL_DEMANGLE_HPP

#include <string>
#include <typeinfo>

// https://gist.github.com/brimston3/2be168bb423c83b0f469c0be56e66d31
#if defined(__clang__)
  #if __has_feature(cxx_rtti)
    #define TAO_PQ_HAS_RTTI
  #endif
#elif defined(__GNUG__)
  #if defined(__GXX_RTTI)
    #define TAO_PQ_HAS_RTTI
  #endif
#elif defined(_MSC_VER)
  #if defined(_CPPRTTI)
    #define TAO_PQ_HAS_RTTI
  #endif
#endif

namespace tao::pq::internal
{
   [[nodiscard]] auto demangle( const char* const symbol ) -> std::string;

   [[nodiscard]] inline auto demangle( const std::type_info& type_info )
   {
      return demangle( type_info.name() );
   }

   template< typename T >
   [[nodiscard]] auto demangle()
   {
#if defined(TAO_PQ_HAS_RTTI)
      return internal::demangle( typeid( T ) );
#else
      return std::string{ "(no rtti)" };
#endif
   }

}  // namespace tao::pq::internal

#endif
