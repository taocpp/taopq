// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_POOL_HPP
#define TAO_PQ_INTERNAL_POOL_HPP

#include <cassert>
#include <list>
#include <memory>
#include <mutex>
#include <utility>

namespace tao::pq::internal
{
   template< typename T >
   class pool
      : public std::enable_shared_from_this< pool< T > >
   {
   private:
      std::list< std::shared_ptr< T > > m_items;
      std::mutex m_mutex;

      struct deleter
      {
         std::weak_ptr< pool > m_pool;

         deleter() = default;

         explicit deleter( std::weak_ptr< pool >&& p ) noexcept
            : m_pool( std::move( p ) )
         {}

         void operator()( T* item ) const noexcept
         {
            std::unique_ptr< T > up( item );
            if( const auto p = m_pool.lock() ) {
               p->push( up );
            }
         }
      };

   protected:
      pool() = default;
      virtual ~pool() = default;

      // create a new T
      [[nodiscard]] virtual auto v_create() const -> std::unique_ptr< T > = 0;
      [[nodiscard]] virtual auto v_is_valid( T& ) const noexcept -> bool = 0;

      void push( std::unique_ptr< T >& up ) noexcept
      {
         if( v_is_valid( *up ) ) {
            std::shared_ptr< T > sp( up.release(), deleter() );
            const std::lock_guard lock( m_mutex );
            // potentially throws -> calls abort() due to noexcept!
            m_items.emplace_back( std::move( sp ) );
         }
      }

      [[nodiscard]] auto pull() noexcept
      {
         std::shared_ptr< T > nrv;
         const std::lock_guard lock( m_mutex );
         if( !m_items.empty() ) {
            nrv = std::move( m_items.back() );
            m_items.pop_back();
         }
         return nrv;
      }

   public:
      pool( const pool& ) = delete;
      pool( pool&& ) = delete;
      void operator=( const pool& ) = delete;
      void operator=( pool&& ) = delete;

      static void attach( const std::shared_ptr< T >& sp, std::weak_ptr< pool >&& p ) noexcept
      {
         deleter* d = std::get_deleter< deleter >( sp );
         assert( d );
         d->m_pool = std::move( p );
      }

      static void detach( const std::shared_ptr< T >& sp ) noexcept
      {
         deleter* d = std::get_deleter< deleter >( sp );
         assert( d );
         d->m_pool.reset();
      }

      // create a new T which is put into the pool when no longer used
      [[nodiscard]] auto create() -> std::shared_ptr< T >
      {
         return { v_create().release(), deleter( this->weak_from_this() ) };
      }

      // get an instance from the pool or create a new one if necessary
      [[nodiscard]] auto get() -> std::shared_ptr< T >
      {
         while( const auto sp = pull() ) {
            if( v_is_valid( *sp ) ) {
               attach( sp, this->weak_from_this() );
               return sp;
            }
         }
         return create();
      }

      void erase_invalid()
      {
         std::list< std::shared_ptr< T > > deferred_delete;
         const std::lock_guard lock( m_mutex );
         auto it = m_items.begin();
         while( it != m_items.end() ) {
            if( !v_is_valid( **it ) ) {
               deferred_delete.splice( deferred_delete.end(), m_items, it++ );
            }
            else {
               ++it;
            }
         }
      }
   };

}  // namespace tao::pq::internal

#endif
