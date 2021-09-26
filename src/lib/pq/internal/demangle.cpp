// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#if !defined( _WIN32 )
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

#include <tao/pq/internal/demangle.hpp>

namespace tao::pq::internal
{
   auto demangle( const char* const symbol ) -> std::string
   {
#if defined( _WIN32 )
      return symbol;
#else
      const std::unique_ptr< char, decltype( &std::free ) > demangled( abi::__cxa_demangle( symbol, nullptr, nullptr, nullptr ), &std::free );
      return demangled ? demangled.get() : symbol;
#endif
   }

}  // namespace tao::pq::internal
