// Copyright (c) 2020-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_ISOLATION_LEVEL_HPP
#define TAO_PQ_ISOLATION_LEVEL_HPP

#include <cstdint>

namespace tao::pq
{
   enum class isolation_level : std::uint8_t
   {
      default_isolation_level,
      serializable,
      repeatable_read,
      read_committed,
      read_uncommitted
   };

}  // namespace tao::pq

#endif
