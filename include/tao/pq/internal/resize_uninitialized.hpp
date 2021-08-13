// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_RESIZE_UNINITIALIZED_HPP
#define TAO_PQ_INTERNAL_RESIZE_UNINITIALIZED_HPP

#include <cstddef>
#include <string>

namespace tao::pq::internal
{
   void resize_uninitialized_impl( std::string& v, const std::size_t n );
   void resize_uninitialized_impl( std::basic_string< unsigned char >& v, const std::size_t n );
   void resize_uninitialized_impl( std::basic_string< std::byte >& v, const std::size_t n );

   template< typename T >
   void resize_uninitialized( std::basic_string< T >& v, const std::size_t n )
   {
      if( n <= v.size() ) {
         v.resize( n );
      }
      else {
         // careful not to call reserve() unless necessary, as it causes shrink_to_fit() on many platforms
         if( n > v.capacity() ) {
            v.reserve( n );
         }
         resize_uninitialized_impl( v, n );
      }
   }

}  // namespace tao::pq::internal

#endif
