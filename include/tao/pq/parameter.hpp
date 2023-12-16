// Copyright (c) 2023 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_PARAMETER_HPP
#define TAO_PQ_PARAMETER_HPP

#include <cassert>
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
      struct binder
      {
         binder() noexcept = default;

         binder( const binder& ) = delete;
         binder( binder&& ) = delete;

         void operator=( const binder& ) = delete;
         void operator=( binder&& ) = delete;

         virtual ~binder() = default;
      };

      template< typename T >
      class traits_binder : public binder
      {
      private:
         const parameter_traits< T > m_traits;

      public:
         explicit traits_binder( const T& t ) noexcept( noexcept( parameter_traits< T >( t ) ) )
            : m_traits( t )
         {}

         template< std::size_t... Is >
         void fill( Oid* types, const char** values, int* lengths, int* formats, std::index_sequence< Is... > /*unused*/ ) const  // TODO: noexcept( ... )?
         {
            ( ( types[ Is ] = static_cast< Oid >( m_traits.template type< Is >() ) ), ... );
            ( ( values[ Is ] = m_traits.template value< Is >() ), ... );
            ( ( lengths[ Is ] = m_traits.template length< Is >() ), ... );
            ( ( formats[ Is ] = m_traits.template format< Is >() ), ... );
         }
      };

      std::size_t m_pos = 0;
      std::unique_ptr< binder > m_binder[ Max ];

      int m_size = 0;
      Oid m_types[ Max ];
      const char* m_values[ Max ];
      int m_lengths[ Max ];
      int m_formats[ Max ];

      friend class transaction;

      template< typename A >
      void bind_impl( const A& a )  // TODO: protect against binding temporaries!
      {
         constexpr auto columns = parameter_traits< std::decay_t< const A& > >::columns;
         if( ( static_cast< std::size_t >( m_size ) + columns ) > Max ) {
            throw std::length_error( "too many parameters!" );
         }

         auto* bptr = new traits_binder< std::decay_t< const A& > >( a );
         m_binder[ m_pos++ ].reset( bptr );

         bptr->fill( &m_types[ m_size ], &m_values[ m_size ], &m_lengths[ m_size ], &m_formats[ m_size ], std::make_index_sequence< columns >() );
         m_size += columns;
      }

   public:
      // NOTE: arguments must remain VALID and UNMODIFIED until this object is destroyed or reset.
      template< typename... As >
      void bind( As&&... as )
      {
         ( bind_impl( std::forward< As >( as ) ), ... );
      }

      void reset() noexcept
      {
         for( std::size_t i = 0; i < m_pos; ++i ) {
            m_binder[ i ].reset();
         }
         m_pos = 0;
         m_size = 0;
      }
   };

}  // namespace tao::pq

#endif
