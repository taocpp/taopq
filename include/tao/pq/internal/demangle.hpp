// The Art of C++ / taopq
// Copyright (c) 2016-2019 Daniel Frey

#ifndef TAO_PQ_INTERNAL_DEMANGLE_HPP
#define TAO_PQ_INTERNAL_DEMANGLE_HPP

#include <string>
#include <typeinfo>

namespace tao::pq::internal
{
   [[nodiscard]] std::string demangle( const char* const symbol );
   [[nodiscard]] std::string demangle( const std::type_info& type_info );

   template< typename T >
   [[nodiscard]] std::string demangle()
   {
      return demangle( typeid( T ) );
   }

}  // namespace tao::pq::internal

#endif
