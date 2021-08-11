// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/internal/resize_uninitialized.hpp>

namespace tao::pq::internal
{
   namespace
   {
      template< typename Tag >
      struct result
      {
         using type = typename Tag::type;
         static type ptr;
      };

      template< typename Tag >
      typename result< Tag >::type result< Tag >::ptr;

      template< typename T, typename T::type P >
      struct steal : result< T >
      {
         /* fill it ... */
         struct filler
         {
            filler()
            {
               result< T >::ptr = P;
            }
         };
         static filler filler_obj;
      };

      template< typename T, typename T::type P >
      typename steal< T, P >::filler steal< T, P >::filler_obj;

      struct string_set_size
      {
         using type = void ( std::string::* )( std::size_t );
      };

      struct binary_set_size
      {
         using type = void ( std::basic_string< std::byte >::* )( std::size_t );
      };

#if defined( _LIBCPP_STRING )

      template class steal< string_set_size, &std::string::__set_size >;
      template class steal< binary_set_size, &std::basic_string< std::byte >::__set_size >;

#elif defined( _GLIBCXX_STRING ) && _GLIBCXX_USE_CXX11_ABI

      template class steal< string_set_size, &std::string::_M_set_length >;
      template class steal< binary_set_size, &std::basic_string< std::byte >::_M_set_length >;

#elif defined( _MSC_VER )

      template class steal< string_set_size, &std::string::_Eos >;
      template class steal< binary_set_size, &std::basic_string< std::byte >::_Eos >;

#else
#warning "No implementation for resize_uninitialized available."
#endif
   }  // namespace

   void resize_uninitialized_impl( std::string& v, const std::size_t n )
   {
      ( v.*result< string_set_size >::ptr )( n );
   }

   void resize_uninitialized_impl( std::basic_string< std::byte >& v, const std::size_t n )
   {
      ( v.*result< binary_set_size >::ptr )( n );
   }

}  // namespace tao::pq::internal
