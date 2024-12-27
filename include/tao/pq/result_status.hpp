// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_RESULT_STATUS_HPP
#define TAO_PQ_RESULT_STATUS_HPP

#include <cstdint>
#include <string_view>

#include <libpq-fe.h>

#include <tao/pq/internal/format_as.hpp>

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

   [[nodiscard]] inline constexpr auto taopq_format_as( const result_status rs ) noexcept -> std::string_view
   {
      switch( rs ) {
         case result_status::empty_query:
            return "empty_query";

         case result_status::command_ok:
            return "command_ok";

         case result_status::tuples_ok:
            return "tuples_ok";

         case result_status::copy_out:
            return "copy_out";

         case result_status::copy_in:
            return "copy_in";

         case result_status::bad_response:
            return "bad_response";

         case result_status::nonfatal_error:
            return "nonfatal_error";

         case result_status::fatal_error:
            return "fatal_error";

         case result_status::single_tuple:
            return "single_tuple";

#if defined( LIBPQ_HAS_CHUNK_MODE )
         case result_status::tuples_chunk:
            return "tuples_chunk";
#endif

         case result_status::pipeline_sync:
            return "pipeline_sync";

         case result_status::pipeline_aborted:
            return "pipeline_aborted";

         default:
            return "<unknown>";
      }
   }

}  // namespace tao::pq

#endif
