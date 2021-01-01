// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_ISOLATION_LEVEL_HPP
#define TAO_PQ_ISOLATION_LEVEL_HPP

namespace tao::pq
{
   enum class isolation_level
   {
      default_isolation_level,
      serializable,
      repeatable_read,
      read_committed,
      read_uncommitted
   };

}  // namespace tao::pq

#endif
