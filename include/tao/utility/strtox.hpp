// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_UTILITY_STRTOX_HPP
#define TAO_UTILITY_STRTOX_HPP

namespace tao
{
   namespace utility
   {
      long strtol( const char* input, const int base = 0 );
      unsigned long strtoul( const char* input, const int base = 0 );
      long long strtoll( const char* input, const int base = 0 );
      unsigned long long strtoull( const char* input, const int base = 0 );

      float strtof( const char* input );
      double strtod( const char* input );
      long double strtold( const char* input );

   }  // namespace utility

}  // namespace tao

#endif
