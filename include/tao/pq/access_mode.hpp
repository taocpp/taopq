// Copyright (c) 2021-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_ACCESS_MODE_HPP
#define TAO_PQ_ACCESS_MODE_HPP

#include <cstdint>
#include <string_view>

#include <tao/pq/internal/format_as.hpp>

namespace tao::pq
{
   enum class access_mode : std::uint8_t
   {
      default_access_mode,
      read_write,
      read_only
   };

   [[nodiscard]] constexpr auto taopq_format_as( const access_mode am ) noexcept -> std::string_view
   {
      switch( am ) {
         case access_mode::default_access_mode:
            return "default_access_mode";

         case access_mode::read_write:
            return "read_write";

         case access_mode::read_only:
            return "read_only";

         default:
            return "<unknown>";
      }
   }

}  // namespace tao::pq

#endif
