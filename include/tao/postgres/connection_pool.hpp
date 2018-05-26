// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_CONNECTION_POOL_HPP
#define TAO_POSTGRES_CONNECTION_POOL_HPP

#include <memory>
#include <string>
#include <utility>

#include <tao/utility/pool.hpp>

#include <tao/postgres/connection.hpp>
#include <tao/postgres/result.hpp>

namespace tao
{
   namespace postgres
   {
      class connection_pool
         : public utility::pool< postgres::connection >
      {
      private:
         const std::string connection_info_;

         std::unique_ptr< postgres::connection > v_create() const override;
         bool v_is_valid( postgres::connection& c ) const override;

      public:
         static std::shared_ptr< connection_pool > create( const std::string& connection_info );

      private:
         // pass-key idiom
         class private_key
         {
            private_key() = default;
            friend std::shared_ptr< connection_pool > connection_pool::create( const std::string& connect_info );
         };

      public:
         connection_pool( const private_key&, const std::string& connection_info );

         std::shared_ptr< postgres::connection > connection()
         {
            return this->get();
         }

         template< typename... Ts >
         result execute( Ts&&... ts )
         {
            return this->connection()->direct()->execute( std::forward< Ts >( ts )... );
         }
      };

   }  // namespace postgres

}  // namespace tao

#endif
