// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_RESIZE_UNINITIALIZED_HPP
#define TAO_PQ_INTERNAL_RESIZE_UNINITIALIZED_HPP

#include <cstddef>
#include <string>

namespace tao::pq::internal
{
   // the below uses a hack to call private member functions of a class, described here:
   // https://github.com/facebook/folly/blob/master/folly/memory/UninitializedMemoryHacks.h

   namespace  // NOLINT(google-build-namespaces)
   {
      void resize_uninitialized_proxy( std::string& v, const std::size_t n );

#if defined( _LIBCPP_STRING )

      // ...create a proxy to generate the actual implementation of the above function...
      template< typename T, void ( T::*F )( std::size_t ) >
      struct proxy
      {
         // ...define the function declared above...
         friend void resize_uninitialized_proxy( T& v, const std::size_t n )
         {
            ( v.*F )( n );  // v.__set_size( n );
            v[ v.size() ] = typename T::value_type( 0 );
         }
      };

      // ...and here's the actual "trick": an explicit template instantiation skips the access checks,
      // so you can reference private members and forward them to the above proxy!
      template struct proxy< std::string, &std::string::__set_size >;

#elif defined( _GLIBCXX_STRING ) && _GLIBCXX_USE_CXX11_ABI

      template< typename T, void ( T::*F )( std::size_t ) >
      struct proxy
      {
         friend void resize_uninitialized_proxy( T& v, const std::size_t n )
         {
            ( v.*F )( n );  // v._M_set_length( n );
         }
      };

      template struct proxy< std::string, &std::string::_M_set_length >;

#elif defined( _GLIBCXX_STRING )

      template< typename T,
                typename R,
                R* ( T::*F )() const >
      struct proxy
      {
         friend void resize_uninitialized_proxy( T& v, const std::size_t n )
         {
            // v._M_rep()->_M_set_length_and_sharable( n );
            ( v.*F )()->_M_set_length_and_sharable( n );
         }
      };

      template struct proxy< std::string,
                             std::string::_Rep,
                             &std::string::_M_rep >;

#elif defined( _MSC_VER )

      template< typename T, void ( T::*F )( std::size_t ) >
      struct proxy
      {
         friend void resize_uninitialized_proxy( T& v, const std::size_t n )
         {
            ( v.*F )( n );  // v._Eos( n );
         }
      };

      template struct proxy< std::string, &std::string::_Eos >;

#else
#error "No implementation for resize_uninitialized available."
#endif

   }  // namespace

   inline void resize_uninitialized( std::string& v, const std::size_t n )
   {
      if( n <= v.size() ) {
         v.resize( n );
      }
      else {
         if( n > v.capacity() ) {
            v.reserve( n );
         }
         internal::resize_uninitialized_proxy( v, n );
      }
   }

}  // namespace tao::pq::internal

#endif
