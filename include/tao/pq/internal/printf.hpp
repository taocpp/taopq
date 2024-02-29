// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_PRINTF_HPP
#define TAO_PQ_INTERNAL_PRINTF_HPP

#include <cstdarg>
#include <string>

namespace tao::pq::internal
{
#if defined( _MSC_VER )

   [[nodiscard]] std::string printf( const char* format, ... );          // NOLINT(modernize-use-trailing-return-type)
   [[nodiscard]] std::string vprintf( const char* format, va_list ap );  // NOLINT(modernize-use-trailing-return-type)

#else

   // clang-format off
   [[nodiscard]] std::string printf( const char* format, ... ) __attribute__(( format( printf, 1, 2 ) ));          // NOLINT(modernize-use-trailing-return-type)
   [[nodiscard]] std::string vprintf( const char* format, va_list ap ) __attribute__(( format( printf, 1, 0 ) ));  // NOLINT(modernize-use-trailing-return-type)
   // clang-format on

#endif

}  // namespace tao::pq::internal

#endif
