// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_LOG_HPP
#define TAO_PQ_LOG_HPP

#include <chrono>
#include <functional>

#include <libpq-fe.h>

#include <tao/pq/poll.hpp>

namespace tao::pq
{
   class connection;

   struct log
   {
      struct connection_pool_t
      {
         // TODO...

      } connection_pool;

      struct connection_t
      {
         using send_query_t = std::function< void( connection&, const char* statement, int n_params, const Oid types[], const char* const values[], const int lengths[], const int formats[] ) >;
         using send_query_result_t = std::function< void( connection&, int result ) >;

         using send_query_prepared_t = std::function< void( connection&, const char* statement, int n_params, const char* const values[], const int lengths[], const int formats[] ) >;
         using send_query_prepared_result_t = std::function< void( connection&, int result ) >;

         using wait_t = std::function< void( connection&, bool wait_for_write, std::chrono::steady_clock::time_point end ) >;

         using poll_t = std::function< void( connection&, int socket, bool wait_for_write, int timeout_ms ) >;
         using poll_result_t = std::function< void( connection&, int socket, poll::status status ) >;

         using is_busy_result_t = std::function< void( const connection&, int result ) >;  // noexcept

         using consume_input_t = std::function< void( connection& ) >;
         using consume_input_result_t = std::function< void( connection&, int result ) >;

         using flush_t = std::function< void( connection& ) >;
         using flush_result_t = std::function< void( connection&, int result ) >;

         using get_result_t = std::function< void( connection&, std::chrono::steady_clock::time_point end ) >;
         using get_result_result_t = std::function< void( connection&, PGresult* ) >;

         struct : send_query_t
         {
            send_query_result_t result;
            using send_query_t::operator=;
         } send_query;

         struct : send_query_prepared_t
         {
            send_query_prepared_result_t result;
            using send_query_prepared_t::operator=;
         } send_query_prepared;

         wait_t wait;

         struct : poll_t
         {
            poll_result_t result;
            using poll_t::operator=;
         } poll;

         struct
         {
            is_busy_result_t result;
         } is_busy;

         struct : consume_input_t
         {
            consume_input_result_t result;
            using consume_input_t::operator=;
         } consume_input;

         struct : flush_t
         {
            flush_result_t result;
            using flush_t::operator=;
         } flush;

         struct : get_result_t
         {
            get_result_result_t result;
            using get_result_t::operator=;
         } get_result;

      } connection;

      struct transaction_t
      {
         // TODO...

      } transaction;
   };

}  // namespace tao::pq

#endif
