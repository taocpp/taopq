// Copyright (c) 2021-2023 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_ZSV_HPP
#define TAO_PQ_INTERNAL_ZSV_HPP

#include <cstddef>
#include <string>
#include <string_view>

namespace tao::pq::internal
{
   // zero-terminated string view
   struct zsv
   {
      const char* value;

      zsv( std::nullptr_t ) = delete;

      constexpr zsv( const char* v ) noexcept
         : value( v )
      {}

      zsv( const std::string& v ) noexcept
         : value( v.c_str() )
      {}

      constexpr operator const char*() const noexcept
      {
         return value;
      }

      constexpr operator std::string_view() const noexcept
      {
         return value;
      }
   };

}  // namespace tao::pq::internal

#endif
