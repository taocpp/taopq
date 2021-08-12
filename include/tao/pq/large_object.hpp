// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_LARGE_OBJECT_HPP
#define TAO_PQ_LARGE_OBJECT_HPP

#include <cstdint>
#include <ios>
#include <memory>

#include <tao/pq/binary.hpp>
#include <tao/pq/oid.hpp>

namespace tao::pq
{
   namespace internal
   {
      class transaction;

   }  // namespace internal

   class large_object
   {
   private:
      // TODO: Should we block this transaction (see table_reader)
      std::shared_ptr< internal::transaction > m_transaction;
      int m_fd;

   public:
      [[nodiscard]] static auto create( const std::shared_ptr< internal::transaction >& transaction, const oid desired_id = 0 ) -> oid;

      static void remove( const std::shared_ptr< internal::transaction >& transaction, const oid id );

      [[nodiscard]] static auto import_file( const std::shared_ptr< internal::transaction >& transaction, const char* filename, const oid desired_id = 0 ) -> oid;
      static void export_file( const std::shared_ptr< internal::transaction >& transaction, const oid id, const char* filename );

      large_object( const std::shared_ptr< internal::transaction >& transaction, const oid id, const std::ios_base::openmode m );

      large_object( const large_object& ) = delete;
      large_object( large_object&& other ) noexcept;

      ~large_object();

      auto operator=( const large_object& ) -> large_object& = delete;
      auto operator=( large_object&& rhs ) -> large_object&;  // NOLINT(performance-noexcept-move-constructor)

      void close();

      [[nodiscard]] auto read( std::byte* data, const std::size_t size ) -> std::size_t;

      template< typename T = binary >
      [[nodiscard]] auto read( const std::size_t size ) -> T = delete;

      void write( const binary_view data );

      template< typename... Ts >
      void write( Ts&&... ts )
      {
         write( pq::to_binary_view( std::forward< Ts >( ts )... ) );
      }

      void resize( const std::int64_t size );

      void seek( const std::int64_t offset, const std::ios_base::seekdir whence );
      [[nodiscard]] auto tell() const -> std::int64_t;
   };

   template<>
   [[nodiscard]] auto large_object::read< binary >( const std::size_t size ) -> binary;

   template<>
   [[nodiscard]] auto large_object::read< std::string >( const std::size_t size ) -> std::string;

}  // namespace tao::pq

#endif
