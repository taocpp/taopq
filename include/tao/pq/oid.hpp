// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_OID_HPP
#define TAO_PQ_OID_HPP

#include <libpq-fe.h>

namespace tao::pq
{
   using oid = Oid;
   constexpr oid invalid_oid = InvalidOid;

}  // namespace tao::pq

#endif
