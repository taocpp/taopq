// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_ZSV_HPP
#define TAO_PQ_INTERNAL_ZSV_HPP

#include <cstddef>
#include <string>

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
   };

}  // namespace tao::pq::internal

#endif
