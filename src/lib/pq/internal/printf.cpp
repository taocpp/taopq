// Copyright (c) 2016-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <cassert>
#include <cstdarg>

#include <tao/pq/internal/printf.hpp>
#include <tao/pq/internal/unreachable.hpp>

namespace tao::pq::internal
{
   namespace
   {
      class va_deleter  // NOLINT(cppcoreguidelines-special-member-functions)
      {
      private:
         va_list& m_ap;

      public:
         explicit va_deleter( va_list& ap )
            : m_ap( ap )
         {}

         ~va_deleter()
         {
            va_end( m_ap );  // NOLINT(clang-analyzer-valist.Uninitialized)
         }
      };

      [[nodiscard]] auto vnprintf( char* buffer, const std::size_t size, const char* format, va_list ap ) -> std::size_t
      {
         const int result = ::vsnprintf( buffer, size, format, ap );
         if( result < 0 ) {
            assert( !"unexpected result from ::vsnprintf()" );  // LCOV_EXCL_LINE
         }
         return static_cast< std::size_t >( result );
      }

      [[nodiscard]] auto vnprintf( std::string& s, const std::size_t size, const char* format, va_list ap ) -> std::size_t
      {
         s.resize( size );
         const auto result = vnprintf( &s[ 0 ], size, format, ap );
         if( result < size ) {
            s.resize( result );
         }
         return result;
      }

   }  // namespace

   std::string printf( const char* format, ... )  // NOLINT(modernize-use-trailing-return-type)
   {
      va_list ap;
      va_start( ap, format );
      va_deleter d( ap );
      return vprintf( format, ap );
   }

   std::string vprintf( const char* format, va_list ap )  // NOLINT(modernize-use-trailing-return-type)
   {
      std::string s;
      const std::size_t initial_size = s.capacity();
      va_list ap2;
      va_copy( ap2, ap );
      va_deleter d( ap2 );
      const std::size_t result = vnprintf( s, initial_size, format, ap2 );
      if( result >= initial_size ) {
         if( vnprintf( s, result + 1, format, ap ) >= result + 1 ) {
            TAO_PQ_UNREACHABLE;  // LCOV_EXCL_LINE
         }
      }
      return s;
   }

}  // namespace tao::pq::internal
