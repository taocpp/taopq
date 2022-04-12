// Copyright (c) 2021-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/large_object.hpp>

#include <cassert>
#include <utility>

#include <libpq/libpq-fs.h>

#include <tao/pq/connection.hpp>
#include <tao/pq/internal/resize_uninitialized.hpp>
#include <tao/pq/transaction.hpp>

namespace tao::pq
{
   namespace
   {
      [[nodiscard]] constexpr auto to_mode( const std::ios_base::openmode m ) noexcept -> int
      {
         return ( ( ( m & std::ios_base::in ) != 0 ) ? INV_READ : 0 ) | ( ( ( m & std::ios_base::out ) != 0 ) ? INV_WRITE : 0 );
      }

   }  // namespace

   auto large_object::create( const std::shared_ptr< transaction >& transaction, const oid desired_id ) -> oid
   {
      const oid id = static_cast< oid >( lo_create( transaction->connection()->underlying_raw_ptr(), static_cast< Oid >( desired_id ) ) );
      if( id == oid::invalid ) {
         throw std::runtime_error( "tao::pq::large_object::create() failed: " + transaction->connection()->error_message() );
      }
      return id;
   }

   void large_object::remove( const std::shared_ptr< transaction >& transaction, const oid id )
   {
      if( lo_unlink( transaction->connection()->underlying_raw_ptr(), static_cast< Oid >( id ) ) == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::remove() failed: " + transaction->connection()->error_message() );
      }
   }

   auto large_object::import_file( const std::shared_ptr< transaction >& transaction, const char* filename, const oid desired_id ) -> oid
   {
      const oid id = static_cast< oid >( lo_import_with_oid( transaction->connection()->underlying_raw_ptr(), filename, static_cast< Oid >( desired_id ) ) );
      if( id == oid::invalid ) {
         throw std::runtime_error( "tao::pq::large_object::import_file() failed: " + transaction->connection()->error_message() );
      }
      return id;
   }

   void large_object::export_file( const std::shared_ptr< transaction >& transaction, const oid id, const char* filename )
   {
      if( lo_export( transaction->connection()->underlying_raw_ptr(), static_cast< Oid >( id ), filename ) == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::export_file() failed: " + transaction->connection()->error_message() );
      }
   }

   large_object::large_object( const std::shared_ptr< transaction >& transaction, const oid id, const std::ios_base::openmode m )
      : m_transaction( transaction ),
        m_fd( lo_open( transaction->connection()->underlying_raw_ptr(), static_cast< Oid >( id ), to_mode( m ) ) )
   {
      if( m_fd == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::open() failed: " + transaction->connection()->error_message() );
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
         // TODO: How to handle this case properly?
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
         if( lo_close( m_transaction->connection()->underlying_raw_ptr(), m_fd ) == -1 ) {
            throw std::runtime_error( "tao::pq::large_object::close() failed: " + m_transaction->connection()->error_message() );
         }
         m_transaction.reset();
      }
   }

   auto large_object::read( char* data, const std::size_t size ) -> std::size_t
   {
      assert( m_transaction );
      const auto result = lo_read( m_transaction->connection()->underlying_raw_ptr(), m_fd, data, size );
      if( result == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::read() failed: " + m_transaction->connection()->error_message() );
      }
      return result;
   }

   void large_object::write( const char* data, const std::size_t size )
   {
      assert( m_transaction );
      if( lo_write( m_transaction->connection()->underlying_raw_ptr(), m_fd, data, size ) == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::write() failed: " + m_transaction->connection()->error_message() );
      }
   }

   void large_object::resize( const std::int64_t size )
   {
      assert( m_transaction );
      if( lo_truncate64( m_transaction->connection()->underlying_raw_ptr(), m_fd, size ) == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::resize() failed: " + m_transaction->connection()->error_message() );
      }
   }

   auto large_object::seek( const std::int64_t offset, const std::ios_base::seekdir whence ) -> std::int64_t
   {
      static_assert( std::ios_base::beg == SEEK_SET );
      static_assert( std::ios_base::cur == SEEK_CUR );
      static_assert( std::ios_base::end == SEEK_END );

      assert( m_transaction );
      const auto result = lo_lseek64( m_transaction->connection()->underlying_raw_ptr(), m_fd, offset, whence );
      if( result == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::seek() failed: " + m_transaction->connection()->error_message() );
      }
      return result;
   }

   auto large_object::tell() const -> std::int64_t
   {
      assert( m_transaction );
      const auto pos = lo_tell64( m_transaction->connection()->underlying_raw_ptr(), m_fd );
      if( pos == -1 ) {
         throw std::runtime_error( "tao::pq::large_object::tell() failed: " + m_transaction->connection()->error_message() );
      }
      return pos;
   }

   template<>
   auto large_object::read< std::string >( const std::size_t size ) -> std::string
   {
      std::string nrv;
      internal::resize_uninitialized( nrv, size );
      nrv.resize( read( nrv.data(), size ) );
      return nrv;
   }

   template<>
   auto large_object::read< std::basic_string< unsigned char > >( const std::size_t size ) -> std::basic_string< unsigned char >
   {
      std::basic_string< unsigned char > nrv;
      internal::resize_uninitialized( nrv, size );
      nrv.resize( read( nrv.data(), size ) );
      return nrv;
   }

   template<>
   auto large_object::read< binary >( const std::size_t size ) -> binary
   {
      binary nrv;
      internal::resize_uninitialized( nrv, size );
      nrv.resize( read( nrv.data(), size ) );
      return nrv;
   }

}  // namespace tao::pq
