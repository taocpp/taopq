// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_PQ_INTERNAL_STRTOX_HPP
#define TAO_PQ_INTERNAL_STRTOX_HPP

namespace tao
{
   namespace pq
   {
      namespace internal
      {
         [[nodiscard]] long strtol( const char* input, const int base = 0 );
         [[nodiscard]] unsigned long strtoul( const char* input, const int base = 0 );
         [[nodiscard]] long long strtoll( const char* input, const int base = 0 );
         [[nodiscard]] unsigned long long strtoull( const char* input, const int base = 0 );

         [[nodiscard]] float strtof( const char* input );
         [[nodiscard]] double strtod( const char* input );
         [[nodiscard]] long double strtold( const char* input );

      }  // namespace internal

   }  // namespace pq

}  // namespace tao

#endif
