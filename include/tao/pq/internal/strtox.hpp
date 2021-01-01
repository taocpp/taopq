// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_STRTOX_HPP
#define TAO_PQ_INTERNAL_STRTOX_HPP

namespace tao::pq::internal
{
   [[nodiscard]] auto strtol( const char* input, const int base = 0 ) -> long;
   [[nodiscard]] auto strtoul( const char* input, const int base = 0 ) -> unsigned long;
   [[nodiscard]] auto strtoll( const char* input, const int base = 0 ) -> long long;
   [[nodiscard]] auto strtoull( const char* input, const int base = 0 ) -> unsigned long long;

   [[nodiscard]] auto strtof( const char* input ) -> float;
   [[nodiscard]] auto strtod( const char* input ) -> double;
   [[nodiscard]] auto strtold( const char* input ) -> long double;

}  // namespace tao::pq::internal

#endif
