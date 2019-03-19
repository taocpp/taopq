// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_PRINTF_HPP
#define TAO_PQ_INTERNAL_PRINTF_HPP

#include <cstdarg>
#include <string>

namespace tao::pq::internal
{
#ifdef WIN32
   [[nodiscard]] std::string printf( const char* format, ... );
   [[nodiscard]] std::string vprintf( const char* format, va_list ap );
#else
   // clang-format off
   [[nodiscard]] std::string printf( const char* format, ... ) __attribute__(( format( printf, 1, 2 ) ));
   [[nodiscard]] std::string vprintf( const char* format, va_list ap ) __attribute__(( format( printf, 1, 0 ) ));
   // clang-format on
#endif

}  // namespace tao::pq::internal

#endif
