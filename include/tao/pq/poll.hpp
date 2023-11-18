// Copyright (c) 2023 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_POLL_HPP
#define TAO_PQ_POLL_HPP

namespace tao::pq
{
   namespace poll
   {
      enum class status
      {
         timeout,
         readable,
         writable,
         again
      };

      using callback = status( const int socket, const bool wait_for_write, const int timeout );

   }  // namespace poll

   namespace internal
   {
      [[nodiscard]] auto poll( const int socket, const bool wait_for_write, const int timeout ) -> pq::poll::status;

   }  // namespace internal

}  // namespace tao::pq

#endif
