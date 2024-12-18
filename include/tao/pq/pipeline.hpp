// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_PIPELINE_HPP
#define TAO_PQ_PIPELINE_HPP

#include <memory>

#include <tao/pq/transaction_base.hpp>

namespace tao::pq
{
   class connection;

   class pipeline
      : public transaction_base
   {
   private:
      std::shared_ptr< transaction_base > m_previous;

   public:
      explicit pipeline( const std::shared_ptr< pq::connection >& connection );

      ~pipeline() override
      {
         try {
            finish();
         }
         // LCOV_EXCL_START
         catch( ... ) {  // NOLINT(bugprone-empty-catch)
            // TODO: How to handle this case properly?
         }
         // LCOV_EXCL_STOP
      }

      pipeline( const pipeline& ) = delete;
      pipeline( pipeline&& ) = delete;
      void operator=( const pipeline& ) = delete;
      void operator=( pipeline&& ) = delete;

      void sync();
      void consume_sync();

      void finish();
   };

}  // namespace tao::pq

#endif
