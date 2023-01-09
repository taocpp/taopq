// Copyright (c) 2016-2023 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_STRTOX_HPP
#define TAO_PQ_INTERNAL_STRTOX_HPP

namespace tao::pq::internal
{
   [[nodiscard]] auto strtof( const char* input ) -> float;
   [[nodiscard]] auto strtod( const char* input ) -> double;
   [[nodiscard]] auto strtold( const char* input ) -> long double;

}  // namespace tao::pq::internal

#endif
