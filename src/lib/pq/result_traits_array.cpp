// Copyright (c) 2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/result_traits_array.hpp>

#include <cstring>
#include <stdexcept>
#include <string>

#include <tao/pq/internal/unreachable.hpp>

namespace tao::pq::internal
{
   auto parse_quoted( const char*& value ) -> std::string
   {
      std::string result;
      while( const auto* pos = std::strpbrk( value, "\\\"" ) ) {
         switch( *pos ) {
            case '\\':
               result.append( value, pos++ );
               result += *pos++;
               value = pos;
               break;

            case '"':
               result.append( value, pos++ );
               value = pos;
               return result;

            default:
               TAO_PQ_INTERNAL_UNREACHABLE;
         }
      }
      throw std::invalid_argument( "unterminated quoted string" );
   }

}  // namespace tao::pq::internal
