// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_LARGE_OBJECT_HPP
#define TAO_PQ_LARGE_OBJECT_HPP

#include <cstdint>
#include <cstring>
#include <ios>
#include <memory>

#include <tao/pq/binary.hpp>
#include <tao/pq/oid.hpp>

namespace tao::pq
{
   class transaction;

   class large_object final
   {
   private:
      std::shared_ptr< transaction > m_transaction;
      int m_fd;

   public:
      [[nodiscard]] static auto create( const std::shared_ptr< transaction >& transaction, const oid desired_id = oid::invalid ) -> oid;

      static void remove( const std::shared_ptr< transaction >& transaction, const oid id );

      [[nodiscard]] static auto import_file( const std::shared_ptr< transaction >& transaction, const char* filename, const oid desired_id = oid::invalid ) -> oid;
      static void export_file( const std::shared_ptr< transaction >& transaction, const oid id, const char* filename );

      large_object( const std::shared_ptr< transaction >& transaction, const oid id, const std::ios_base::openmode m );

      large_object( const large_object& ) = delete;
      large_object( large_object&& other ) noexcept;

      ~large_object();

      void operator=( const large_object& ) = delete;
      auto operator=( large_object&& rhs ) -> large_object&;  // NOLINT(performance-noexcept-move-constructor)

      void close();

      [[nodiscard]] auto read( char* data, const std::size_t size ) -> std::size_t;
      void write( const char* data, const std::size_t size );

      [[nodiscard]] auto read( std::byte* data, const std::size_t size ) -> std::size_t
      {
         return read( reinterpret_cast< char* >( data ), size );
      }

      void write( const std::byte* data, const std::size_t size )
      {
         write( reinterpret_cast< const char* >( data ), size );
      }

      template< typename T = binary >
      [[nodiscard]] auto read( const std::size_t size ) -> T = delete;

      void write( const char* data )
      {
         write( data, std::strlen( data ) );
      }

      template< typename... Ts >
      void write( Ts&&... ts )
      {
         const auto bv = pq::to_binary_view( std::forward< Ts >( ts )... );
         write( bv.data(), bv.size() );
      }

      void resize( const std::int64_t size );

      auto seek( const std::int64_t offset, const std::ios_base::seekdir whence ) -> std::int64_t;
      [[nodiscard]] auto tell() const -> std::int64_t;
   };

   template<>
   [[nodiscard]] auto large_object::read< std::string >( const std::size_t size ) -> std::string;

   template<>
   [[nodiscard]] auto large_object::read< binary >( const std::size_t size ) -> binary;

}  // namespace tao::pq

#endif
