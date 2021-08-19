// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/internal/resize_uninitialized.hpp>

namespace tao::pq::internal
{
   namespace
   {
      void resize_uninitialized_proxy( std::string& v, const std::size_t n );
      void resize_uninitialized_proxy( std::basic_string< unsigned char >& v, const std::size_t n );
      void resize_uninitialized_proxy( std::basic_string< std::byte >& v, const std::size_t n );

#if defined( _LIBCPP_STRING )

      template< typename T, void ( T::*F )( std::size_t ) >
      struct proxy
      {
         friend void resize_uninitialized_proxy( T& v, const std::size_t n )
         {
            ( v.*F )( n );
            v[ v.size() ] = typename T::value_type( 0 );
         }
      };

      template class proxy< std::string, &std::string::__set_size >;
      template class proxy< std::basic_string< unsigned char >, &std::basic_string< unsigned char >::__set_size >;
      template class proxy< std::basic_string< std::byte >, &std::basic_string< std::byte >::__set_size >;

#elif defined( _GLIBCXX_STRING ) && _GLIBCXX_USE_CXX11_ABI

      template< typename T, void ( T::*F )( std::size_t ) >
      struct proxy
      {
         friend void resize_uninitialized_proxy( T& v, const std::size_t n )
         {
            ( v.*F )( n );
         }
      };

      template class proxy< std::string, &std::string::_M_set_length >;
      template class proxy< std::basic_string< unsigned char >, &std::basic_string< unsigned char >::_M_set_length >;
      template class proxy< std::basic_string< std::byte >, &std::basic_string< std::byte >::_M_set_length >;

#elif defined( _GLIBCXX_STRING )

      template< typename T, typename R, R* ( T::*G )() const, void ( R::*F )( std::size_t ) >
      struct proxy
      {
         friend void resize_uninitialized_proxy( T& v, const std::size_t n )
         {
            auto* rep = ( v.*G )();
            ( rep->*F )( n );
         }
      };

      template class proxy< std::string,
                            std::string::_Rep,
                            &std::string::_M_rep,
                            &std::string::_Rep::_M_set_length_and_sharable >;

      template class proxy< std::basic_string< unsigned char >,
                            std::basic_string< unsigned char >::_Rep,
                            &std::basic_string< unsigned char >::_M_rep,
                            &std::basic_string< unsigned char >::_Rep::_M_set_length_and_sharable >;

      template class proxy< std::basic_string< std::byte >,
                            std::basic_string< std::byte >::_Rep,
                            &std::basic_string< std::byte >::_M_rep,
                            &std::basic_string< std::byte >::_Rep::_M_set_length_and_sharable >;

#elif defined( _MSC_VER )

      template< typename T, void ( T::*F )( std::size_t ) >
      struct proxy
      {
         friend void resize_uninitialized_proxy( T& v, const std::size_t n )
         {
            ( v.*F )( n );
         }
      };

      template class proxy< std::string, &std::string::_Eos >;
      template class proxy< std::basic_string< unsigned char >, &std::basic_string< unsigned char >::_Eos >;
      template class proxy< std::basic_string< std::byte >, &std::basic_string< std::byte >::_Eos >;

#else
#warning "No implementation for resize_uninitialized available."
#endif

   }  // namespace

   void resize_uninitialized_impl( std::string& v, const std::size_t n )
   {
      resize_uninitialized_proxy( v, n );
   }

   void resize_uninitialized_impl( std::basic_string< unsigned char >& v, const std::size_t n )
   {
      resize_uninitialized_proxy( v, n );
   }

   void resize_uninitialized_impl( std::basic_string< std::byte >& v, const std::size_t n )
   {
      resize_uninitialized_proxy( v, n );
   }

}  // namespace tao::pq::internal
