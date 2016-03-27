// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#include <tao/utility/demangle.hpp>

#include <memory>
#include <cstdlib>
#include <cxxabi.h>

namespace tao
{
  namespace utility
  {
    std::string demangle( const char* const symbol )
    {
      const std::unique_ptr< char, decltype( &std::free ) > demangled( abi::__cxa_demangle( symbol, nullptr, nullptr, nullptr ), &std::free );
      return demangled ? demangled.get() : symbol;
    }

    std::string demangle( const std::type_info& type_info )
    {
      return demangle( type_info.name() );
    }
  }
}
