// Copyright (c) 2022-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_CONNECTION_STATUS_HPP
#define TAO_PQ_CONNECTION_STATUS_HPP

#include <cstdint>
#include <string_view>

#include <libpq-fe.h>

#include <tao/pq/internal/format_as.hpp>

namespace tao::pq
{
   enum class connection_status : std::uint8_t
   {
      ok = CONNECTION_OK,
      bad = CONNECTION_BAD
   };

   [[nodiscard]] constexpr auto taopq_format_as( const connection_status cs ) noexcept -> std::string_view
   {
      switch( cs ) {
         case connection_status::ok:
            return "ok";

         case connection_status::bad:
            return "bad";

         default:
            return "<unknown>";
      }
   }

}  // namespace tao::pq

#endif
