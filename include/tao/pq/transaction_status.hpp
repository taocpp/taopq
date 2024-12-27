// Copyright (c) 2022-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_TRANSACTION_STATUS_HPP
#define TAO_PQ_TRANSACTION_STATUS_HPP

#include <cstdint>
#include <string_view>

#include <libpq-fe.h>

#include <tao/pq/internal/format_as.hpp>

namespace tao::pq
{
   enum class transaction_status : std::uint8_t
   {
      idle = PQTRANS_IDLE,
      in_transaction = PQTRANS_INTRANS,
      active = PQTRANS_ACTIVE,
      error = PQTRANS_INERROR,
      unknown = PQTRANS_UNKNOWN
   };

   [[nodiscard]] constexpr auto taopq_format_as( const transaction_status ts ) noexcept -> std::string_view
   {
      switch( ts ) {
         case transaction_status::idle:
            return "idle";

         case transaction_status::in_transaction:
            return "in_transaction";

         case transaction_status::active:
            return "active";

         case transaction_status::error:
            return "error";

         case transaction_status::unknown:
            return "unknown";

         default:
            return "<unknown>";
      }
   }

}  // namespace tao::pq

#endif
