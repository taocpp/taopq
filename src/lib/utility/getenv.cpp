// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#include <tao/utility/getenv.hpp>

#include <cstdlib>
#include <stdexcept>

namespace tao
{
  namespace utility
  {
    std::string getenv( const std::string& name )
    {
      const char* result = std::getenv( name.c_str() );
      return result ? result : throw std::runtime_error( "environment variable not found: " + name );
    }

    std::string getenv( const std::string& name, const std::string& default_value )
    {
      const char* result = std::getenv( name.c_str() );
      return result ? result : default_value;
    }
  }
}
