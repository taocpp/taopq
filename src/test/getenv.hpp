// Copyright (c) 2016-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef SRC_TEST_GETENV_HPP  // NOLINT(llvm-header-guard)
#define SRC_TEST_GETENV_HPP

// This is an internal header used for unit-tests.

#include <cstdlib>
#include <format>
#include <stdexcept>
#include <string>

#if defined( _MSC_VER )
#include <memory>
#endif

namespace tao::pq::internal
{
#if defined( _MSC_VER )

   [[nodiscard]] inline auto getenv( const std::string& name ) -> std::string
   {
      char* buf = nullptr;
      std::size_t sz = 0;
      if( _dupenv_s( &buf, &sz, name.c_str() ) == 0 && buf != nullptr ) {
         const std::unique_ptr< char, decltype( &std::free ) > up( buf, &std::free );
         return std::string( up.get(), sz );
      }
      throw std::runtime_error( std::format( "environment variable not found: {}", name ) );
   }

   [[nodiscard]] inline auto getenv( const std::string& name, const std::string& default_value ) -> std::string
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

   [[nodiscard]] inline auto getenv( const std::string& name ) -> std::string
   {
      const char* result = std::getenv( name.c_str() );
      return ( result != nullptr ) ? result : throw std::runtime_error( std::format( "environment variable not found: {}", name ) );
   }

   [[nodiscard]] inline auto getenv( const std::string& name, const std::string& default_value ) -> std::string
   {
      const char* result = std::getenv( name.c_str() );
      return ( result != nullptr ) ? result : default_value;
   }

#endif

}  // namespace tao::pq::internal

#endif
