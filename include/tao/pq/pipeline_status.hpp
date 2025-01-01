// Copyright (c) 2024-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_PIPELINE_STATUS_HPP
#define TAO_PQ_PIPELINE_STATUS_HPP

#include <cstdint>
#include <string_view>

#include <libpq-fe.h>

#include <tao/pq/internal/format_as.hpp>

namespace tao::pq
{
   enum class pipeline_status : std::uint8_t
   {
      on = PQ_PIPELINE_ON,
      off = PQ_PIPELINE_OFF,
      aborted = PQ_PIPELINE_ABORTED
   };

   [[nodiscard]] constexpr auto taopq_format_as( const pipeline_status ps ) noexcept -> std::string_view
   {
      switch( ps ) {
         case pipeline_status::on:
            return "on";

         case pipeline_status::off:
            return "off";

         case pipeline_status::aborted:
            return "aborted";

         default:
            return "<unknown>";
      }
   }

}  // namespace tao::pq

#endif
