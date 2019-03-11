// The Art of C++ / taopq
// Copyright (c) 2016-2019 Daniel Frey

#ifndef TAO_PQ_NULL_HPP
#define TAO_PQ_NULL_HPP

namespace tao::pq
{
   struct null_t
   {
      explicit constexpr null_t( int /*unused*/ ) {}
   };

   constexpr null_t null{ 0 };

}  // namespace tao::pq

#endif
