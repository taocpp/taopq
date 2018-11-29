// The Art of C++ / taopq
// Copyright (c) 2016-2018 Daniel Frey

#ifndef SRC_TEST_GETENV_HPP
#define SRC_TEST_GETENV_HPP

// This is an internal header used for unit-tests.

#include <cstdlib>
#include <stdexcept>
#include <string>

#if defined( _MSC_VER )
#include <memory>
#endif

namespace tao
{
   namespace pq
   {
      namespace internal
      {
#if defined( _MSC_VER )
         [[nodiscard]] inline std::string getenv( const std::string& name )
         {
            char* buf = nullptr;
            std::size_t sz = 0;
            if( _dupenv_s( &buf, &sz, name.c_str() ) == 0 && buf != nullptr ) {
               const std::unique_ptr< char, decltype( &std::free ) > up( buf, &std::free );
               return std::string( up.get(), sz );
            }
            throw std::runtime_error( "environment variable not found: " + name );
         }

         [[nodiscard]] inline std::string getenv( const std::string& name, const std::string& default_value )
         {
            char* buf = nullptr;
            std::size_t sz = 0;
            if( _dupenv_s( &buf, &sz, name.c_str() ) == 0 && buf != nullptr ) {
               const std::unique_ptr< char, decltype( &std::free ) > up( buf, &std::free );
               return std::string( up.get(), sz );
            }
            return default_value;
         }
#else
         [[nodiscard]] inline std::string getenv( const std::string& name )
         {
            const char* result = std::getenv( name.c_str() );
            return result ? result : throw std::runtime_error( "environment variable not found: " + name );
         }

         [[nodiscard]] inline std::string getenv( const std::string& name, const std::string& default_value )
         {
            const char* result = std::getenv( name.c_str() );
            return result ? result : default_value;
         }
#endif

      }  // namespace internal

   }  // namespace pq

}  // namespace tao

#endif
