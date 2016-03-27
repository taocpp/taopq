// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_UTILITY_STRTOX_HPP
#define TAOCPP_INCLUDE_UTILITY_STRTOX_HPP

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
  }
}

#endif // TAOCPP_INCLUDE_UTILITY_STRTOX_HPP
