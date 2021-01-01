// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_CONNECTION_HPP
#define TAO_PQ_INTERNAL_CONNECTION_HPP

#include <functional>
#include <memory>
#include <set>
#include <string>

#include <libpq-fe.h>

#include <tao/pq/result.hpp>

namespace tao::pq
{
   class table_writer;

   namespace internal
   {
      class transaction;

      struct deleter final
      {
         void operator()( PGconn* p ) const noexcept
         {
            PQfinish( p );
         }
      };

      class connection
         : public std::enable_shared_from_this< connection >
      {
      private:
         friend class transaction;
         friend class pq::table_writer;

         const std::unique_ptr< PGconn, deleter > m_pgconn;
         transaction* m_current_transaction;
         std::set< std::string, std::less<> > m_prepared_statements;

         [[nodiscard]] auto error_message() const -> std::string;
         static void check_prepared_name( const std::string& name );
         [[nodiscard]] auto is_prepared( const char* name ) const noexcept -> bool;

         [[nodiscard]] auto execute_params( const char* statement,
                                            const int n_params,
                                            const Oid types[],
                                            const char* const values[],
                                            const int lengths[],
                                            const int formats[] ) -> result;

      protected:
         explicit connection( const std::string& connection_info );

      public:
         connection( const connection& ) = delete;
         connection( connection&& ) = delete;
         void operator=( const connection& ) = delete;
         void operator=( connection&& ) = delete;

         ~connection() = default;

         [[nodiscard]] auto is_open() const noexcept -> bool;

         void prepare( const std::string& name, const std::string& statement );
         void deallocate( const std::string& name );

         [[nodiscard]] auto underlying_raw_ptr() noexcept -> PGconn*
         {
            return m_pgconn.get();
         }

         [[nodiscard]] auto underlying_raw_ptr() const noexcept -> const PGconn*
         {
            return m_pgconn.get();
         }
      };

   }  // namespace internal

}  // namespace tao::pq

#endif
