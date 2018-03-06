// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_NULL_HPP
#define TAO_POSTGRES_NULL_HPP

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

   }  // namespace postgres

}  // namespace tao

#endif
