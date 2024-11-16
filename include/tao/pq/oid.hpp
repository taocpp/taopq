// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_OID_HPP
#define TAO_PQ_OID_HPP

#include <libpq-fe.h>

namespace tao::pq
{
   static_assert( InvalidOid == 0 );

   enum class oid : Oid
   {
      invalid = 0,
      bytea = 17,
      text = 25
   };

}  // namespace tao::pq

#endif
