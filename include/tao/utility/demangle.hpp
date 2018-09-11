// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_UTILITY_DEMANGLE_HPP
#define TAO_UTILITY_DEMANGLE_HPP

#include <string>
#include <typeinfo>

#include <postgres_export.h>

namespace tao
{
   namespace utility
   {
      POSTGRES_EXPORT std::string demangle( const char* const symbol );
      POSTGRES_EXPORT std::string demangle( const std::type_info& type_info );

      template< typename T >
      std::string demangle()
      {
         return demangle( typeid( T ) );
      }

   }  // namespace utility

}  // namespace tao

#endif
