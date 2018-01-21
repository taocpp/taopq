// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#include <tao/utility/getenv.hpp>

#include <cstdlib>
#include <stdexcept>

namespace tao
{
   namespace utility
   {
#if defined( _MSC_VER )
      std::string getenv( const std::string& name )
      {
         char* buf = nullptr;
         if( _dupenv_s( &buf, nullptr, name.c_str() ) == 0 && buf != nullptr ) {
            const std::unique_ptr< char, decltype( &std::free ) > up( buf, &std::free );
            return up.get();
         }
         throw std::runtime_error( "environment variable not found: " + name );
      }

      std::string getenv( const std::string& name, const std::string& default_value )
      {
         char* buf = nullptr;
         if( _dupenv_s( &buf, nullptr, name.c_str() ) == 0 && buf != nullptr ) {
            const std::unique_ptr< char, decltype( &std::free ) > up( buf, &std::free );
            return up.get();
         }
         return default_value;
      }
#else
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
#endif
   }
}
