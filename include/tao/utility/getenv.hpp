// The Art of C++ / Utility
// Copyright (c) 2016-2017 Daniel Frey

#ifndef TAOCPP_INCLUDE_UTILITY_GETENV_HPP
#define TAOCPP_INCLUDE_UTILITY_GETENV_HPP

#include <string>

namespace tao
{
   namespace utility
   {
      std::string getenv( const std::string& name );
      std::string getenv( const std::string& name, const std::string& default_value );
   }
}

#endif  // TAOCPP_INCLUDE_UTILITY_GETENV_HPP
