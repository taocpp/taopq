// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_UTILITY_DEMANGLE_HPP
#define TAO_UTILITY_DEMANGLE_HPP

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

   }  // namespace utility

}  // namespace tao

#endif
