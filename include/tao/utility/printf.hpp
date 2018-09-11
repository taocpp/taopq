// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_UTILITY_PRINTF_HPP
#define TAO_UTILITY_PRINTF_HPP

#include <cstdarg>
#include <string>

#include <postgres_export.h>

namespace tao
{
   namespace utility
   {
#ifdef WIN32
      POSTGRES_EXPORT std::string printf( const char* format, ... );
      POSTGRES_EXPORT std::string vprintf( const char* format, va_list ap );
#else
      // clang-format off
      POSTGRES_EXPORT std::string printf( const char* format, ... ) __attribute__(( format( printf, 1, 2 ) ));
      POSTGRES_EXPORT std::string vprintf( const char* format, va_list ap ) __attribute__(( format( printf, 1, 0 ) ));
      // clang-format on
#endif

   }  // namespace utility

}  // namespace tao

#endif
