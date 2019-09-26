// Copyright (c) 2016-2019 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <cassert>
#include <cstdarg>

#include <tao/pq/internal/printf.hpp>

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
         {
         }

         ~va_deleter()
         {
            va_end( m_ap );  // NOLINT(clang-analyzer-valist.Uninitialized)
         }
      };

      [[nodiscard]] int vnprintf( std::string& s, const std::size_t size, const char* format, va_list ap )
      {
         s.resize( size );
         char* buffer = &s[ 0 ];
         const int result = ::vsnprintf( buffer, size, format, ap );
         if( result < 0 ) {
            assert( !"unexpected result from ::vsnprintf()" );  // LCOV_EXCL_LINE
         }
         if( static_cast< std::size_t >( result ) < size ) {
            s.resize( result );
         }
         return result;
      }

   }  // namespace

   std::string printf( const char* format, ... )
   {
      va_list ap;
      va_start( ap, format );
      va_deleter d( ap );
      return vprintf( format, ap );
   }

   std::string vprintf( const char* format, va_list ap )
   {
      std::string s;
      static constexpr int initial_size = 32;
      va_list ap2;
      va_copy( ap2, ap );
      va_deleter d( ap2 );
      const int result = vnprintf( s, initial_size, format, ap2 );
      if( result >= initial_size ) {
         if( vnprintf( s, result + 1, format, ap ) >= result + 1 ) {
            assert( !"code should be unreachable" );  // LCOV_EXCL_LINE
         }
      }
      return s;
   }

}  // namespace tao::pq::internal
