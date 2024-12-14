// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_STATUS_HPP
#define TAO_PQ_RESULT_STATUS_HPP

#include <cstdint>

#include <libpq-fe.h>

namespace tao::pq
{
   enum class result_status : std::uint8_t
   {
      empty_query = PGRES_EMPTY_QUERY,
      command_ok = PGRES_COMMAND_OK,
      tuples_ok = PGRES_TUPLES_OK,
      copy_out = PGRES_COPY_OUT,
      copy_in = PGRES_COPY_IN,
      bad_response = PGRES_BAD_RESPONSE,
      nonfatal_error = PGRES_NONFATAL_ERROR,
      fatal_error = PGRES_FATAL_ERROR,
      single_tuple = PGRES_SINGLE_TUPLE,
#if defined( LIBPQ_HAS_CHUNK_MODE )
      tuples_chunk = PGRES_TUPLES_CHUNK,
#endif
      pipeline_sync = PGRES_PIPELINE_SYNC,
      pipeline_aborted = PGRES_PIPELINE_ABORTED
   };

}  // namespace tao::pq

#endif
