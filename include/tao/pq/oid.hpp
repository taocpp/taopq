// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

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
