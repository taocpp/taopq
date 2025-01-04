// Copyright (c) 2024-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef SRC_TEST_COMPARE_HPP  // NOLINT(llvm-header-guard)
#define SRC_TEST_COMPARE_HPP

// This is an internal header used for unit-tests.

#include <cstddef>

namespace tao::pq::internal
{
   template< typename T, typename U >
   [[nodiscard]] auto compare( const T& lhs, const U& rhs ) noexcept -> bool
   {
      if( lhs.size() != rhs.size() ) {
         return false;
      }
      for( std::size_t i = 0; i != lhs.size(); ++i ) {
         if( lhs[ i ] != rhs[ i ] ) {
            return false;
         }
      }
      return true;
   }

}  // namespace tao::pq::internal

#endif
