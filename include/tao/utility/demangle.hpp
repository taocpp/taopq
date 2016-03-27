// The Art of C++ / Utility
// Copyright (c) 2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_UTILITY_DEMANGLE_HPP
#define TAOCPP_INCLUDE_UTILITY_DEMANGLE_HPP

#include <string>
#include <typeinfo>

namespace tao
{
  namespace utility
  {
    std::string demangle( const char* const symbol );
    std::string demangle( const std::type_info& type_info );

    template< typename T >
    std::string demangle()
    {
      return demangle( typeid( T ) );
    }
  }
}

#endif // TAOCPP_INCLUDE_UTILITY_DEMANGLE_HPP
