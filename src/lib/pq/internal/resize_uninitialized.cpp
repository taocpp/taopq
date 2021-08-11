// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/internal/resize_uninitialized.hpp>

namespace tao::pq::internal
{
   namespace
   {
      template< typename T >
      struct proxy
      {
         static typename T::type value;
      };

      template< typename T >
      typename T::type proxy< T >::value;

      template< typename T, typename T::type P >
      struct set_proxy
      {
         struct setter
         {
            setter()
            {
               proxy< T >::value = P;
            }
         };
         static setter instance;
      };

      template< typename T, typename T::type P >
      typename set_proxy< T, P >::setter set_proxy< T, P >::instance;

      struct string_set_size
      {
         using type = void ( std::string::* )( std::size_t );
      };

      struct binary_set_size
      {
         using type = void ( std::basic_string< std::byte >::* )( std::size_t );
      };

#if defined( _LIBCPP_STRING )

      template class set_proxy< string_set_size, &std::string::__set_size >;
      template class set_proxy< binary_set_size, &std::basic_string< std::byte >::__set_size >;

#elif defined( _GLIBCXX_STRING )

      template class set_proxy< string_set_size, &std::string::_M_set_length >;
      template class set_proxy< binary_set_size, &std::basic_string< std::byte >::_M_set_length >;

#elif defined( _MSC_VER )

      template class set_proxy< string_set_size, &std::string::_Eos >;
      template class set_proxy< binary_set_size, &std::basic_string< std::byte >::_Eos >;

#else
#warning "No implementation for resize_uninitialized available."
#endif

   }  // namespace

   void resize_uninitialized_impl( std::string& v, const std::size_t n )
   {
      ( v.*proxy< string_set_size >::value )( n );
   }

   void resize_uninitialized_impl( std::basic_string< std::byte >& v, const std::size_t n )
   {
      ( v.*proxy< binary_set_size >::value )( n );
   }

}  // namespace tao::pq::internal
