// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_UTILITY_STRTOX_HPP
#define TAO_UTILITY_STRTOX_HPP

#include <postgres_export.h>

namespace tao
{
   namespace utility
   {
      POSTGRES_EXPORT long strtol( const char* input, const int base = 0 );
      POSTGRES_EXPORT unsigned long strtoul( const char* input, const int base = 0 );
      POSTGRES_EXPORT long long strtoll( const char* input, const int base = 0 );
      POSTGRES_EXPORT unsigned long long strtoull( const char* input, const int base = 0 );

      POSTGRES_EXPORT float strtof( const char* input );
      POSTGRES_EXPORT double strtod( const char* input );
      POSTGRES_EXPORT long double strtold( const char* input );

   }  // namespace utility

}  // namespace tao

#endif
