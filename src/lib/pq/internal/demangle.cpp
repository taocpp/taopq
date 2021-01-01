// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef WIN32
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

#include <tao/pq/internal/demangle.hpp>

namespace tao::pq::internal
{
   auto demangle( const char* const symbol ) -> std::string
   {
#ifdef WIN32
      return symbol;
#else
      const std::unique_ptr< char, decltype( &std::free ) > demangled( abi::__cxa_demangle( symbol, nullptr, nullptr, nullptr ), &std::free );
      return demangled ? demangled.get() : symbol;
#endif
   }

   auto demangle( const std::type_info& type_info ) -> std::string
   {
      return demangle( type_info.name() );
   }

}  // namespace tao::pq::internal
