// The Art of C++ / PostgreSQL
// Copyright (c) 2016 Daniel Frey

#include <tao/postgres/parameter_traits.hpp>

namespace tao
{
  namespace postgres
  {
    template<> const char* const parameter_traits< signed char >::format = "%hhd";
    template<> const char* const parameter_traits< unsigned char >::format = "%hhu";
    template<> const char* const parameter_traits< short >::format = "%hd";
    template<> const char* const parameter_traits< unsigned short >::format = "%hu";
    template<> const char* const parameter_traits< int >::format = "%d";
    template<> const char* const parameter_traits< unsigned >::format = "%u";
    template<> const char* const parameter_traits< long >::format = "%ld";
    template<> const char* const parameter_traits< unsigned long >::format = "%lu";
    template<> const char* const parameter_traits< long long >::format = "%lld";
    template<> const char* const parameter_traits< unsigned long long >::format = "%llu";

    template<> const char* const parameter_traits< float >::format = "%.*g";
    template<> const char* const parameter_traits< double >::format = "%.*g";
    template<> const char* const parameter_traits< long double >::format = "%.*Lg";
  }
}
