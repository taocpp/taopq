// Copyright (c) 2023 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_PARAMETER_HPP
#define TAO_PQ_PARAMETER_HPP

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <tao/pq/oid.hpp>
#include <tao/pq/parameter_traits.hpp>

namespace tao::pq
{
   class transaction;

   template< std::size_t Max = 16 >
   class parameter
   {
   private:
      struct vbase
      {
         vbase() noexcept = default;
         virtual ~vbase() = default;

         vbase( const vbase& ) = delete;
         vbase( vbase&& ) = delete;

         void operator=( const vbase& ) = delete;
         void operator=( vbase&& ) = delete;
      };

      template< typename T >
      struct binder : vbase
      {
         const parameter_traits< T > m_traits;

         explicit binder( const T& t ) noexcept( noexcept( parameter_traits< T >( t ) ) )
            : m_traits( t )
         {}
      };

      template< typename T >
      struct holder : vbase
      {
         const T m_value;
         const parameter_traits< T > m_traits;

         explicit holder( T&& t ) noexcept( noexcept( parameter_traits< T >( t ) ) && std::is_nothrow_move_constructible_v< T > )
            : m_value( std::move( t ) ),
              m_traits( m_value )
         {}
      };

      std::size_t m_pos = 0;
      vbase* m_params[ Max ];

      int m_size = 0;
      Oid m_types[ Max ];
      const char* m_values[ Max ];
      int m_lengths[ Max ];
      int m_formats[ Max ];

      friend class transaction;

      template< typename T, std::size_t... Is >
      void fill( const T& t, std::index_sequence< Is... > /*unused*/ )
      {
         ( ( m_types[ m_size + Is ] = static_cast< Oid >( t.template type< Is >() ) ), ... );
         ( ( m_values[ m_size + Is ] = t.template value< Is >() ), ... );
         ( ( m_lengths[ m_size + Is ] = t.template length< Is >() ), ... );
         ( ( m_formats[ m_size + Is ] = t.template format< Is >() ), ... );
      }

      template< typename A >
      void bind_impl( A&& a )
      {
         using D = std::decay_t< A&& >;

         constexpr auto columns = parameter_traits< D >::columns;
         if( ( static_cast< std::size_t >( m_size ) + columns ) > Max ) {
            throw std::length_error( "too many parameters!" );
         }

         constexpr auto hold = std::is_rvalue_reference_v< A&& > && !parameter_traits< D >::self_contained;
         using container_t = std::conditional_t< hold, holder< D >, binder< D > >;

         auto bptr = std::make_unique< container_t >( std::forward< A >( a ) );
         parameter::fill( bptr->m_traits, std::make_index_sequence< columns >() );

         m_params[ m_pos++ ] = bptr.release();
         m_size += columns;
      }

      template< std::size_t N >
      void bind_impl( const parameter< N >& p )
      {
         const std::size_t columns = p.m_size;
         if( m_size + columns > Max ) {
            throw std::length_error( "too many parameters!" );
         }

         for( std::size_t n = 0; n < columns; ++n ) {
            m_types[ m_size + n ] = p.m_types[ n ];
            m_values[ m_size + n ] = p.m_values[ n ];
            m_lengths[ m_size + n ] = p.m_lengths[ n ];
            m_formats[ m_size + n ] = p.m_formats[ n ];
         }

         m_size += static_cast< int >( columns );
      }

      template< std::size_t N >
      void bind_impl( parameter< N >& p )
      {
         bind_impl( const_cast< const parameter< N >& >( p ) );
      }

   public:  // clang-tidy wants deleted member functions to be public
      template< std::size_t N >
      void bind_impl( parameter< N >&& p ) = delete;

   public:
      template< typename... As >
      explicit parameter( As&&... as ) noexcept( noexcept( std::declval< parameter >().bind( std::forward< As >( as )... ) ) )
      {
         parameter::bind( std::forward< As >( as )... );
      }

      ~parameter()
      {
         for( std::size_t i = 0; i != m_pos; ++i ) {
            delete m_params[ i ];
         }
      }

      parameter( const parameter& p )
      {
         parameter::bind( p );
      }

      parameter( parameter&& p ) = delete;

      void operator=( const parameter& ) = delete;
      void operator=( parameter&& ) = delete;

      template< typename... As >
      void bind( As&&... as ) noexcept( sizeof...( As ) == 0 )
      {
         ( parameter::bind_impl( std::forward< As >( as ) ), ... );
      }

      template< typename... As >
      void reset( As&&... as ) noexcept( noexcept( std::declval< parameter >().bind( std::forward< As >( as )... ) ) )
      {
         for( std::size_t i = 0; i != m_pos; ++i ) {
            delete m_params[ i ];
         }
         m_pos = 0;
         m_size = 0;
         parameter::bind( std::forward< As >( as )... );
      }
   };

}  // namespace tao::pq

#endif
