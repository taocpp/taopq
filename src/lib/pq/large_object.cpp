// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/large_object.hpp>

#include <cassert>
#include <utility>

#include <libpq/libpq-fs.h>

#include <tao/pq/internal/connection.hpp>
#include <tao/pq/internal/transaction.hpp>

namespace tao::pq
{
   namespace
   {
      constexpr auto to_mode( const std::ios_base::openmode m ) noexcept -> int
      {
         return ( ( ( m & std::ios_base::in ) != 0 ) ? INV_READ : 0 ) | ( ( ( m & std::ios_base::out ) != 0 ) ? INV_WRITE : 0 );
      }

   }  // namespace

   auto large_object::create( const std::shared_ptr< internal::transaction >& transaction, const oid desired_id ) -> oid
   {
      const oid id = lo_create( transaction->underlying_raw_ptr(), desired_id );
      if( id == invalid_oid ) {
         throw std::runtime_error( "tao::pq::large_object::create() failed: " + transaction->m_connection->error_message() );
      }
      return id;
   }

   void large_object::remove( const std::shared_ptr< internal::transaction >& transaction, const oid id )
   {
      if( lo_unlink( transaction->underlying_raw_ptr(), id ) == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::remove() failed: " + transaction->m_connection->error_message() );
      }
   }

   auto large_object::import_file( const std::shared_ptr< internal::transaction >& transaction, const char* filename, const oid desired_id ) -> oid
   {
      const oid id = lo_import_with_oid( transaction->underlying_raw_ptr(), filename, desired_id );
      if( id == invalid_oid ) {
         throw std::runtime_error( "tao::pq::large_object::import_file() failed: " + transaction->m_connection->error_message() );
      }
      return id;
   }

   void large_object::export_file( const std::shared_ptr< internal::transaction >& transaction, const oid id, const char* filename )
   {
      if( lo_export( transaction->underlying_raw_ptr(), id, filename ) == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::export_file() failed: " + transaction->m_connection->error_message() );
      }
   }

   large_object::large_object( const std::shared_ptr< internal::transaction >& transaction, const oid id, const std::ios_base::openmode m )
      : m_transaction( transaction ),
        m_fd( lo_open( transaction->underlying_raw_ptr(), id, to_mode( m ) ) )
   {
      if( m_fd == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::open() failed: " + transaction->m_connection->error_message() );
      }
   }

   large_object::large_object( large_object&& other ) noexcept
      : m_transaction( std::move( other.m_transaction ) ),
        m_fd( other.m_fd )
   {}

   large_object::~large_object()
   {
      try {
         close();
      }
      // LCOV_EXCL_START
      catch( ... ) {
         // TODO: We just swallowed an exception...
      }
      // LCOV_EXCL_STOP
   }

   auto large_object::operator=( large_object&& rhs ) -> large_object&  // NOLINT
   {
      close();
      m_transaction = std::move( rhs.m_transaction );
      m_fd = rhs.m_fd;
      return *this;
   }

   void large_object::close()
   {
      if( m_transaction ) {
         if( lo_close( m_transaction->underlying_raw_ptr(), m_fd ) == -1 ) {
            throw std::runtime_error( "tao::pq::large_object::close() failed: " + m_transaction->m_connection->error_message() );
         }
         m_transaction.reset();
      }
   }

   auto large_object::read( const std::size_t len ) -> binary
   {
      assert( m_transaction );
      binary nrv;
      nrv.resize( len );
      const auto result = lo_read( m_transaction->underlying_raw_ptr(), m_fd, reinterpret_cast< char* >( nrv.data() ), len );
      if( result == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::read() failed: " + m_transaction->m_connection->error_message() );
      }
      nrv.resize( result );
      return nrv;
   }

   void large_object::write( const binary_view data )
   {
      assert( m_transaction );
      if( lo_write( m_transaction->underlying_raw_ptr(), m_fd, reinterpret_cast< const char* >( data.data() ), data.size() ) == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::write() failed: " + m_transaction->m_connection->error_message() );
      }
   }

   void large_object::resize( const std::int64_t size )
   {
      assert( m_transaction );
      if( lo_truncate64( m_transaction->underlying_raw_ptr(), m_fd, size ) == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::resize() failed: " + m_transaction->m_connection->error_message() );
      }
   }

   void large_object::seek( const std::int64_t offset, const std::ios_base::seekdir whence )
   {
      static_assert( std::ios_base::beg == SEEK_SET );
      static_assert( std::ios_base::cur == SEEK_CUR );
      static_assert( std::ios_base::end == SEEK_END );

      assert( m_transaction );
      if( lo_lseek64( m_transaction->underlying_raw_ptr(), m_fd, offset, whence ) == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::resize() failed: " + m_transaction->m_connection->error_message() );
      }
   }

   auto large_object::tell() const -> std::int64_t
   {
      assert( m_transaction );
      const auto pos = lo_tell64( m_transaction->underlying_raw_ptr(), m_fd );
      if( pos == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::tell() failed: " + m_transaction->m_connection->error_message() );
      }
      return pos;
   }

}  // namespace tao::pq
