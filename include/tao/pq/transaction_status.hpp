// Copyright (c) 2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_TRANSACTION_STATUS_HPP
#define TAO_PQ_TRANSACTION_STATUS_HPP

#include <libpq-fe.h>

namespace tao::pq
{
   enum class transaction_status
   {
      idle = PQTRANS_IDLE,
      in_transaction = PQTRANS_INTRANS,
      active = PQTRANS_ACTIVE,
      error = PQTRANS_INERROR,
      unknown = PQTRANS_UNKNOWN
   };

}  // namespace tao::pq

#endif
