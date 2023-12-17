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

   // NOTE: for now, this is experimental and might change or vanish at any time!
   // TODO: naming?
   template< std::size_t Max = 16 >
   class parameter
   {
   private:
      struct vbase
      {
         explicit vbase() noexcept = default;
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
      void fill( const T& t, std::index_sequence< Is... > /*unused*/ )  // TODO: noexcept( ... )?
      {
         ( ( m_types[ m_size + Is ] = static_cast< Oid >( t.m_traits.template type< Is >() ) ), ... );
         ( ( m_values[ m_size + Is ] = t.m_traits.template value< Is >() ), ... );
         ( ( m_lengths[ m_size + Is ] = t.m_traits.template length< Is >() ), ... );
         ( ( m_formats[ m_size + Is ] = t.m_traits.template format< Is >() ), ... );
      }

      template< typename A >
      void bind_rvalue_reference( A&& a )
      {
         using D = std::decay_t< A&& >;

         constexpr auto columns = parameter_traits< D >::columns;
         if( ( static_cast< std::size_t >( m_size ) + columns ) > Max ) {
            throw std::length_error( "too many parameters!" );
         }

         auto bptr = std::make_unique< holder< D > >( std::forward< A >( a ) );
         fill( *bptr, std::make_index_sequence< columns >() );

         m_params[ m_pos++ ] = bptr.release();
         m_size += columns;
      }

      template< typename A >
      void bind_const_lvalue_reference( const A& a )
      {
         using D = std::decay_t< const A& >;

         constexpr auto columns = parameter_traits< D >::columns;
         if( ( static_cast< std::size_t >( m_size ) + columns ) > Max ) {
            throw std::length_error( "too many parameters!" );
         }

         auto bptr = std::make_unique< binder< D > >( a );
         fill( *bptr, std::make_index_sequence< columns >() );

         m_params[ m_pos++ ] = bptr.release();
         m_size += columns;
      }

      template< typename A >
      void bind_impl( A&& a )
      {
         using D = std::decay_t< A&& >;
         if constexpr( std::is_rvalue_reference_v< A&& > || parameter_traits< D >::self_contained ) {
            bind_rvalue_reference( std::forward< A >( a ) );
         }
         else {
            bind_const_lvalue_reference( std::forward< A >( a ) );
         }
      }

   public:
      ~parameter()
      {
         for( std::size_t i = 0; i != m_pos; ++i ) {
            delete m_params[ i ];
         }
      }

      // NOTE: arguments must remain VALID and UNMODIFIED until this object is destroyed or reset.
      template< typename... As >
      void bind( As&&... as )
      {
         ( bind_impl( std::forward< As >( as ) ), ... );
      }

      void reset() noexcept
      {
         for( std::size_t i = 0; i != m_pos; ++i ) {
            delete m_params[ i ];
         }
         m_pos = 0;
         m_size = 0;
      }
   };

}  // namespace tao::pq

#endif
