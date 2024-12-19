// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/pipeline.hpp>

#include <chrono>

#include <tao/pq/connection.hpp>

namespace tao::pq
{
   pipeline::pipeline( const std::shared_ptr< pq::connection >& connection )
      : transaction_base( connection ),
        m_previous( current_transaction()->shared_from_this() )
   {
      connection->enter_pipeline_mode();
      current_transaction() = this;
   }

   void pipeline::sync()
   {
      connection()->pipeline_sync();
   }

   void pipeline::consume_sync( const std::chrono::steady_clock::time_point start )
   {
      current_transaction()->consume_pipeline_sync( start );
   }

   void pipeline::finish()
   {
      if( m_previous ) {
         current_transaction() = m_previous.get();
         const auto extend_lifetime = std::move( m_previous );
         connection()->exit_pipeline_mode();
      }
   }

}  // namespace tao::pq
