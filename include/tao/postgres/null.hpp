// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_NULL_HPP
#define TAO_POSTGRES_NULL_HPP

namespace tao
{
   namespace postgres
   {
      struct null_t
      {
         explicit constexpr null_t( int /*unused*/ ) {}
      };

      constexpr null_t null{ 0 };

   }  // namespace postgres

}  // namespace tao

#endif
