// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_STRTOX_HPP
#define TAO_PQ_INTERNAL_STRTOX_HPP

namespace tao::pq::internal
{
   [[nodiscard]] long strtol( const char* input, const int base = 0 );
   [[nodiscard]] unsigned long strtoul( const char* input, const int base = 0 );
   [[nodiscard]] long long strtoll( const char* input, const int base = 0 );
   [[nodiscard]] unsigned long long strtoull( const char* input, const int base = 0 );

   [[nodiscard]] float strtof( const char* input );
   [[nodiscard]] double strtod( const char* input );
   [[nodiscard]] long double strtold( const char* input );

}  // namespace tao::pq::internal

#endif
