// Copyright (c) 2023 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/poll.hpp>

#include <cctype>
#include <cerrno>
#include <cstring>
#include <string>

#if defined( _WIN32 )
#include <winsock2.h>
#else
#include <poll.h>
#endif

#include <tao/pq/exception.hpp>
#include <tao/pq/internal/printf.hpp>
#include <tao/pq/internal/unreachable.hpp>

namespace tao::pq::poll::internal
{
   // LCOV_EXCL_START
   [[nodiscard, maybe_unused]] auto errno_result_to_string( const int e, char* buffer, int result ) -> std::string
   {
      if( result == 0 ) {
         return buffer;
      }
      return pq::internal::printf( "unknown error code %d", e );
   }

   [[nodiscard, maybe_unused]] auto errno_result_to_string( const int /*unused*/, char* /*unused*/, char* result ) -> std::string
   {
      return result;
   }

   [[nodiscard]] auto errno_to_string( const int e ) -> std::string
   {
      char buffer[ 256 ];
#if defined( _WIN32 )
#ifdef _MSC_VER
      return errno_result_to_string( e, buffer, strerror_s( buffer, e ) );
#else
      return errno_result_to_string( e, buffer, strerror_s( buffer, sizeof( buffer ), e ) );
#endif
#else
      return errno_result_to_string( e, buffer, strerror_r( e, buffer, sizeof( buffer ) ) );
#endif
   }
   // LCOV_EXCL_STOP

   auto default_poll( const int socket, const bool wait_for_write, const int timeout ) -> status
   {
#if defined( _WIN32 )

      const short events = POLLIN | ( wait_for_write ? POLLOUT : 0 );
      WSAPOLLFD pfd = { static_cast< SOCKET >( socket ), events, 0 };
      const auto result = WSAPoll( &pfd, 1, timeout );
      switch( result ) {
         case 0:
            return status::timeout;

         case 1:
            if( ( pfd.revents & events ) == 0 ) {
               throw network_error( pq::internal::printf( "WSAPoll() failed, events %hd, revents %hd", events, pfd.revents ) );
            }
            return ( ( pfd.revents & POLLIN ) != 0 ) ? status::readable : status::writable;

         case SOCKET_ERROR: {
            const int e = WSAGetLastError();
            throw network_error( "WSAPoll() failed: " + internal::errno_to_string( e ) );
         }

         default:
            TAO_PQ_UNREACHABLE;
      }

#else

      const short events = POLLIN | ( wait_for_write ? POLLOUT : 0 );
      pollfd pfd = { socket, events, 0 };
      errno = 0;
      const auto result = ::poll( &pfd, 1, timeout );
      switch( result ) {
         case 0:
            return status::timeout;

         case 1:
            if( ( pfd.revents & events ) == 0 ) {
               throw network_error( pq::internal::printf( "poll() failed, events %hd, revents %hd", events, pfd.revents ) );  // LCOV_EXCL_LINE
            }
            return ( ( pfd.revents & POLLIN ) != 0 ) ? status::readable : status::writable;

            // LCOV_EXCL_START
         case -1: {
            const int e = errno;
            if( ( e != EINTR ) && ( e != EAGAIN ) ) {
               throw network_error( "poll() failed: " + internal::errno_to_string( e ) );
            }
            return status::again;
         }

         default:
            TAO_PQ_UNREACHABLE;
      }
         // LCOV_EXCL_STOP
#endif
   }

}  // namespace tao::pq::poll::internal
