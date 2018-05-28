// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_RESULT_HPP
#define TAO_POSTGRES_RESULT_HPP

#include <list>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <tao/optional/optional.hpp>
#include <tao/postgres/row.hpp>
#include <tao/utility/printf.hpp>

// forward-declare libpq structures
struct pg_result;
typedef struct pg_result PGresult;

namespace tao
{
   namespace postgres
   {
      class connection;
      class table_writer;

      namespace result_impl
      {
         template< typename T, typename = void >
         struct has_reserve
            : std::false_type
         {
         };

         template< typename T >
         struct has_reserve< T, typename std::enable_if< std::is_void< decltype( T::reserve( std::declval< typename T::size_type >() ) ) >::value >::type >
            : std::true_type
         {
         };

      }  // namespace result_impl

      class result
      {
      private:
         friend class connection;
         friend class table_writer;

         const std::shared_ptr< PGresult > pgresult_;
         const std::size_t columns_;
         const std::size_t rows_;

         void check_has_result_set() const;
         void check_row( const std::size_t row ) const;

         enum class mode_t
         {
            EXPECT_OK,
            EXPECT_COPY_IN
         };
         result(::PGresult* pgresult, const mode_t mode = mode_t::EXPECT_OK );

      public:
         bool has_rows_affected() const;
         std::size_t rows_affected() const;

         std::size_t columns() const
         {
            return columns_;
         }

         std::string name( const std::size_t column ) const;
         std::size_t index( const std::string& in_name ) const;

         bool empty() const;
         std::size_t size() const;

         class const_iterator
            : private row
         {
         private:
            friend class result;

            const_iterator( const row& r )
               : row( r )
            {
            }

         public:
            friend bool operator!=( const const_iterator& lhs, const const_iterator& rhs )
            {
               return lhs.row_ != rhs.row_;
            }

            const_iterator& operator++()
            {
               ++row_;
               return *this;
            }

            const row& operator*() const
            {
               return *this;
            }
         };

         const_iterator begin() const;
         const_iterator end() const;

         bool is_null( const std::size_t row, const std::size_t column ) const;
         const char* get( const std::size_t row, const std::size_t column ) const;

         row operator[]( const std::size_t row ) const
         {
            return postgres::row( *this, row, 0, columns_ );
         }

         row at( const std::size_t row ) const;

         template< typename T >
         T as() const
         {
            if( size() != 1 ) {
               throw std::runtime_error( utility::printf( "invalid result size: %zu rows, expected 1 row", rows_ ) );
            }
            return ( *this )[ 0 ].as< T >();
         }

         template< typename T >
         tao::optional< T > optional() const
         {
            if( empty() ) {
               return tao::optional< T >();
            }
            return tao::optional< T >( as< T >() );
         }

         template< typename T, typename U >
         std::pair< T, U > pair() const
         {
            return as< std::pair< T, U > >();
         }

         template< typename... Ts >
         std::tuple< Ts... > tuple() const
         {
            return as< std::tuple< Ts... > >();
         }

         template< typename T >
         typename std::enable_if< result_impl::has_reserve< T >::value, T >::type as_container() const
         {
            T nrv;
            nrv.reserve( size() );
            check_has_result_set();
            for( const auto& row : *this ) {
               nrv.insert( nrv.end(), row.as< typename T::value_type >() );
            }
            return nrv;
         }

         template< typename T >
         typename std::enable_if< !result_impl::has_reserve< T >::value, T >::type as_container() const
         {
            T nrv;
            check_has_result_set();
            for( const auto& row : *this ) {
               nrv.insert( nrv.end(), row.as< typename T::value_type >() );
            }
            return nrv;
         }

         template< typename... Ts >
         std::vector< Ts... > vector() const
         {
            return as_container< std::vector< Ts... > >();
         }

         template< typename... Ts >
         std::list< Ts... > list() const
         {
            return as_container< std::list< Ts... > >();
         }

         template< typename... Ts >
         std::set< Ts... > set() const
         {
            return as_container< std::set< Ts... > >();
         }

         template< typename... Ts >
         std::multiset< Ts... > multiset() const
         {
            return as_container< std::multiset< Ts... > >();
         }

         template< typename... Ts >
         std::unordered_set< Ts... > unordered_set() const
         {
            return as_container< std::unordered_set< Ts... > >();
         }

         template< typename... Ts >
         std::unordered_multiset< Ts... > unordered_multiset() const
         {
            return as_container< std::unordered_multiset< Ts... > >();
         }

         template< typename... Ts >
         std::map< Ts... > map() const
         {
            return as_container< std::map< Ts... > >();
         }

         template< typename... Ts >
         std::multimap< Ts... > multimap() const
         {
            return as_container< std::multimap< Ts... > >();
         }

         template< typename... Ts >
         std::unordered_map< Ts... > unordered_map() const
         {
            return as_container< std::unordered_map< Ts... > >();
         }

         template< typename... Ts >
         std::unordered_multimap< Ts... > unordered_multimap() const
         {
            return as_container< std::unordered_multimap< Ts... > >();
         }

         // make sure you include libpq-fe.h before accessing the raw pointer
         ::PGresult* underlying_raw_ptr()
         {
            return pgresult_.get();
         }

         // make sure you include libpq-fe.h before accessing the raw pointer
         const ::PGresult* underlying_raw_ptr() const
         {
            return pgresult_.get();
         }
      };

   }  // namespace postgres

}  // namespace tao

#endif
