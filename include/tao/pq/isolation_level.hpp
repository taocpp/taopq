// Copyright (c) 2020-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_ISOLATION_LEVEL_HPP
#define TAO_PQ_ISOLATION_LEVEL_HPP

#include <cstdint>
#include <string_view>

#include <tao/pq/internal/format_as.hpp>

namespace tao::pq
{
   enum class isolation_level : std::uint8_t
   {
      default_isolation_level,
      serializable,
      repeatable_read,
      read_committed,
      read_uncommitted
   };

   [[nodiscard]] constexpr auto taopq_format_as( const isolation_level il ) noexcept -> std::string_view
   {
      switch( il ) {
         case isolation_level::default_isolation_level:
            return "default_isolation_level";

         case isolation_level::serializable:
            return "serializable";

         case isolation_level::repeatable_read:
            return "repeatable_read";

         case isolation_level::read_committed:
            return "read_committed";

         case isolation_level::read_uncommitted:
            return "read_uncommitted";

         default:
            return "<unknown>";
      }
   }

}  // namespace tao::pq

#endif
