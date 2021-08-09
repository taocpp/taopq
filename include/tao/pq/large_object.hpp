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
      [[nodiscard]] static auto create( const std::shared_ptr< internal::transaction >& transaction, const std::ios_base::openmode m = std::ios_base::in | std::ios_base::out ) -> oid;

      // note: does not throw, may return invalid_oid
      [[nodiscard]] static auto create( const std::shared_ptr< internal::transaction >& transaction, const oid desired_id ) noexcept -> oid;

      static void remove( const std::shared_ptr< internal::transaction >& transaction, const oid id );

      [[nodiscard]] static auto import_file( const std::shared_ptr< internal::transaction >& transaction, const char* filename, const oid desired_id = 0 ) -> oid;
      static void export_file( const std::shared_ptr< internal::transaction >& transaction, const oid id, const char* filename );

   private:
      large_object( const std::shared_ptr< internal::transaction >& transaction, const oid id, const std::ios_base::openmode m );

   public:
      large_object( const large_object& ) = delete;
      large_object( large_object&& other ) noexcept;

      ~large_object();

      large_object& operator=( const large_object& ) = delete;
      large_object& operator=( large_object&& rhs );

      void close();

      auto read( const std::size_t len ) -> binary;
      void write( const binary_view data );

      void resize( const std::int64_t size );

      void seek( const std::int64_t offset, const std::ios_base::seekdir whence );
      auto tell() const -> std::int64_t;
   };

}  // namespace tao::pq

#endif
