// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_UTILITY_POOL_HPP
#define TAO_UTILITY_POOL_HPP

#include <cassert>
#include <list>
#include <memory>
#include <mutex>

namespace tao
{
   namespace utility
   {
      template< typename T >
      class pool
         : public std::enable_shared_from_this< pool< T > >
      {
      private:
         std::list< std::shared_ptr< T > > items_;
         std::mutex mutex_;

         struct deleter
         {
            std::weak_ptr< pool > pool_;

            deleter() noexcept = default;

            explicit deleter( const std::shared_ptr< pool >& p ) noexcept
               : pool_( p )
            {
            }

            void operator()( T* item ) const
            {
               std::unique_ptr< T > up( item );
               if( const auto p = pool_.lock() ) {
                  p->push( up );
               }
            }
         };

      protected:
         pool() = default;
         virtual ~pool() = default;

         // create a new T
         virtual std::unique_ptr< T > v_create() const = 0;
         virtual bool v_is_valid( T& ) const
         {
            return true;
         }

         virtual void v_push_before( T& ) const noexcept
         {
         }
         virtual void v_push_success( T& ) const noexcept
         {
         }
         virtual void v_push_failure( T& ) const noexcept
         {
         }

         virtual void v_pull_before( T& ) const noexcept
         {
         }
         virtual void v_pull_success( T& ) const noexcept
         {
         }
         virtual void v_pull_failure( T& ) const noexcept
         {
         }

         void push( std::unique_ptr< T >& up )
         {
            v_push_before( *up );
            if( v_is_valid( *up ) ) {
               std::shared_ptr< T > sp( up.release(), deleter() );
               v_push_success( *sp );
               const std::lock_guard< std::mutex > lock( mutex_ );
               items_.push_back( std::move( sp ) );
            }
            else {
               v_push_failure( *up );
            }
         }

         std::shared_ptr< T > pull() noexcept
         {
            std::shared_ptr< T > nrv;
            const std::lock_guard< std::mutex > lock( mutex_ );
            if( !items_.empty() ) {
               nrv = std::move( items_.back() );
               items_.pop_back();
            }
            return nrv;
         }

      public:
         pool( const pool& ) = delete;
         void operator=( const pool& ) = delete;

         static void attach( const std::shared_ptr< T >& sp, const std::shared_ptr< pool >& p ) noexcept
         {
            deleter* d = std::get_deleter< deleter >( sp );
            assert( d );
            d->pool_ = p;
         }

         static void detach( const std::shared_ptr< T >& sp ) noexcept
         {
            deleter* d = std::get_deleter< deleter >( sp );
            assert( d );
            d->pool_.reset();
         }

         // create a new T which is put into the pool when no longer used
         std::shared_ptr< T > create()
         {
            return { v_create().release(), deleter( this->shared_from_this() ) };
         }

         // get an instance from the pool or create a new one if necessary
         std::shared_ptr< T > get()
         {
            while( const auto sp = pull() ) {
               v_pull_before( *sp );
               if( v_is_valid( *sp ) ) {
                  attach( sp, this->shared_from_this() );
                  v_pull_success( *sp );
                  return sp;
               }
               else {
                  v_pull_failure( *sp );
               }
            }
            return create();
         }

         void erase_invalid()
         {
            std::list< std::shared_ptr< T > > deferred_delete;
            const std::lock_guard< std::mutex > lock( mutex_ );
            auto it = items_.begin();
            while( it != items_.end() ) {
               if( !v_is_valid( **it ) ) {
                  deferred_delete.push_back( std::move( *it ) );
                  it = items_.erase( it );
               }
               else {
                  ++it;
               }
            }
         }
      };

   }  // namespace utility

}  // namespace tao

#endif
