// Copyright (c) 2023 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_POLL_HPP
#define TAO_PQ_INTERNAL_POLL_HPP

#include <tao/pq/poll.hpp>

namespace tao::pq::internal
{
   [[nodiscard]] auto poll( const int socket, const bool wait_for_write, const int timeout_ms ) -> pq::poll::status;

}  // namespace tao::pq::internal

#endif
