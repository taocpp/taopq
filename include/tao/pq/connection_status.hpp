// Copyright (c) 2022-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_CONNECTION_STATUS_HPP
#define TAO_PQ_CONNECTION_STATUS_HPP

#include <cstdint>

#include <libpq-fe.h>

namespace tao::pq
{
   enum class connection_status : std::uint8_t
   {
      ok = CONNECTION_OK,
      bad = CONNECTION_BAD
   };

}  // namespace tao::pq

#endif
