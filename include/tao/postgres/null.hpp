// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2017 Daniel Frey

#ifndef TAOCPP_INCLUDE_POSTGRES_NULL_HPP
#define TAOCPP_INCLUDE_POSTGRES_NULL_HPP

namespace tao
{
   namespace postgres
   {
      struct null_t final
      {
         constexpr null_t()
         {
         }
         null_t( const null_t& ) = delete;
         void operator=( const null_t& ) = delete;
      };

      constexpr null_t null{};
   }
}

#endif  // TAOCPP_INCLUDE_POSTGRES_NULL_HPP
