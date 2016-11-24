// The Art of C++ / Utility
// Copyright (c) 2016 Daniel Frey

#include <tao/utility/demangle.hpp>

#include <memory>
#include <cstdlib>
#ifndef WIN32
#include <cxxabi.h>
#endif

namespace tao
{
  namespace utility
  {
#ifndef WIN32
    std::string demangle( const char* const symbol )
    {
      const std::unique_ptr< char, decltype( &std::free ) > demangled( abi::__cxa_demangle( symbol, nullptr, nullptr, nullptr ), &std::free );
      return demangled ? demangled.get() : symbol;
    }
#endif

    std::string demangle( const std::type_info& type_info )
    {
      return demangle( type_info.name() );
    }
  }
}
