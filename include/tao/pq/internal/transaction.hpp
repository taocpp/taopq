// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_TRANSACTION_HPP
#define TAO_PQ_INTERNAL_TRANSACTION_HPP

#include <memory>
#include <type_traits>

#include <libpq-fe.h>

#include <tao/pq/result.hpp>

namespace tao::pq
{
   class table_writer;

   namespace internal
   {
      class connection;

      class transaction
         : public std::enable_shared_from_this< transaction >
      {
      public:
         friend class pq::table_writer;

      protected:
         std::shared_ptr< connection > m_connection;

         explicit transaction( const std::shared_ptr< connection >& connection );
         virtual ~transaction() = 0;

      public:
         transaction( const transaction& ) = delete;
         transaction( transaction&& ) = delete;
         void operator=( const transaction& ) = delete;
         void operator=( transaction&& ) = delete;

      protected:
         [[nodiscard]] virtual auto v_is_direct() const noexcept -> bool = 0;

         virtual void v_commit() = 0;
         virtual void v_rollback() = 0;

         virtual void v_reset() noexcept = 0;

         [[nodiscard]] auto current_transaction() const noexcept -> transaction*&;
         void check_current_transaction() const;

         [[nodiscard]] auto execute_params( const char* statement,
                                            const int n_params,
                                            const Oid types[],
                                            const char* const values[],
                                            const int lengths[],
                                            const int formats[] ) -> result;

         auto underlying_raw_ptr() const noexcept -> PGconn*;

      public:
         void commit();
         void rollback();
      };

   }  // namespace internal

}  // namespace tao::pq

#endif
