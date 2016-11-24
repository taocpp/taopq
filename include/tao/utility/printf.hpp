// The Art of C++ / Utility
// Copyright (c) 2016 Daniel Frey

#ifndef TAOCPP_INCLUDE_UTILITY_PRINTF_HPP
#define TAOCPP_INCLUDE_UTILITY_PRINTF_HPP

#include <string>

namespace tao
{
  namespace utility
  {
#ifdef WIN32
#define __attribute__(A) /* do nothing */
#endif
	  std::string printf(const char* format, ...) __attribute__((format(printf, 1, 2)));
	  std::string vprintf(const char* format, va_list ap) __attribute__((format(printf, 1, 0)));
  }
}

#endif // TAOCPP_INCLUDE_UTILITY_PRINTF_HPP
