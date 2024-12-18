// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_TRANSACTION_HPP
#define TAO_PQ_TRANSACTION_HPP

#include <chrono>
#include <memory>
#include <utility>

#include <libpq-fe.h>

#include <tao/pq/internal/zsv.hpp>
#include <tao/pq/parameter.hpp>
#include <tao/pq/transaction_base.hpp>

namespace tao::pq
{
   class pipeline;

   class transaction
      : public transaction_base
   {
   protected:
      using transaction_base::transaction_base;

      [[nodiscard]] virtual auto v_is_direct() const noexcept -> bool = 0;

      virtual void v_commit() = 0;
      virtual void v_rollback() = 0;

      virtual void v_reset() noexcept = 0;

   public:
      [[nodiscard]] auto subtransaction() -> std::shared_ptr< transaction >;
      [[nodiscard]] auto pipeline() -> std::shared_ptr< pq::pipeline >;

      void set_single_row_mode();
#if defined( LIBPQ_HAS_CHUNK_MODE )
      void set_chunk_mode( const int rows );
#endif

      template< parameter_type... As >
      auto execute( const internal::zsv statement, As&&... as )
      {
         const auto start = std::chrono::steady_clock::now();
         transaction_base::send( statement, std::forward< As >( as )... );
         return transaction_base::get_result( start );
      }

      void commit();
      void rollback();
   };

   namespace internal
   {
      class subtransaction_base
         : public transaction
      {
      private:
         const std::shared_ptr< pq::transaction_base > m_previous;

      protected:
         explicit subtransaction_base( const std::shared_ptr< pq::connection >& connection )
            : transaction( connection ),
              m_previous( current_transaction()->shared_from_this() )
         {
            current_transaction() = this;
         }

         ~subtransaction_base() override
         {
            if( m_connection ) {
               current_transaction() = m_previous.get();  // LCOV_EXCL_LINE
            }
         }

         [[nodiscard]] auto v_is_direct() const noexcept -> bool final
         {
            return false;
         }

         void v_reset() noexcept final
         {
            current_transaction() = m_previous.get();
            m_connection.reset();
         }

      public:
         subtransaction_base( const subtransaction_base& ) = delete;
         subtransaction_base( subtransaction_base&& ) = delete;
         void operator=( const subtransaction_base& ) = delete;
         void operator=( subtransaction_base&& ) = delete;
      };

      // blocker for table_reader and table_writer
      class transaction_guard final
         : public subtransaction_base
      {
      public:
         explicit transaction_guard( const std::shared_ptr< pq::connection >& connection )
            : subtransaction_base( connection )
         {}

      private:
         // LCOV_EXCL_START
         void v_commit() override {}
         void v_rollback() override {}
         // LCOV_EXCL_STOP
      };

   }  // namespace internal

}  // namespace tao::pq

#endif
