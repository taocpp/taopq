// Copyright (c) 2023-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_POLL_HPP
#define TAO_PQ_POLL_HPP

#include <cstdint>
#include <string_view>

#include <tao/pq/internal/format_as.hpp>

namespace tao::pq::poll
{
   enum class status : std::uint8_t
   {
      timeout,
      readable,
      writable,
      again
   };

   [[nodiscard]] constexpr auto taopq_format_as( const status st ) noexcept -> std::string_view
   {
      switch( st ) {
         case status::timeout:
            return "timeout";

         case status::readable:
            return "readable";

         case status::writable:
            return "writable";

         case status::again:
            return "again";

         default:
            return "<unknown>";
      }
   }

   using callback = status( const int socket, const bool wait_for_write, const int timeout_ms );

}  // namespace tao::pq::poll

#endif
