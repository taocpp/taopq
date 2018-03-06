// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_UTILITY_MAKE_UNIQUE_HPP
#define TAO_UTILITY_MAKE_UNIQUE_HPP

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace tao
{
   namespace utility
   {
      // C++14's std::make_unique as per N3656 / N3797 + extension

      template< typename T, typename... Args >
      typename std::enable_if< !std::is_array< T >::value, std::unique_ptr< T > >::type
      make_unique( Args&&... args )
      {
         return std::unique_ptr< T >( new T( std::forward< Args >( args )... ) );
      }

      template< typename T >
      typename std::enable_if< std::is_array< T >::value && std::extent< T >::value == 0, std::unique_ptr< T > >::type
      make_unique( const std::size_t n )
      {
         using U = typename std::remove_extent< T >::type;
         return std::unique_ptr< T >( new U[ n ]() );
      }

      template< typename T, typename... Args >
      typename std::enable_if< std::extent< T >::value != 0 >::type
      make_unique( Args&&... )
      {
         static_assert( !( std::extent< T >::value != 0 ), "array with known bound may not be used with make_unique" );
      }

   }  // namespace utility

}  // namespace tao

#endif
