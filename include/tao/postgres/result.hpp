// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef TAO_POSTGRES_RESULT_HPP
#define TAO_POSTGRES_RESULT_HPP

#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

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
         inline constexpr bool has_reserve = false;

         template< typename T >
         inline constexpr bool has_reserve< T, std::void_t< decltype( T::reserve( std::declval< typename T::size_type >() ) ) > > = true;

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
            expect_ok,
            expect_copy_in
         };
         result( ::PGresult* pgresult, const mode_t mode = mode_t::expect_ok );

      public:
         [[nodiscard]] bool has_rows_affected() const;
         [[nodiscard]] std::size_t rows_affected() const;

         [[nodiscard]] std::size_t columns() const
         {
            return columns_;
         }

         [[nodiscard]] std::string name( const std::size_t column ) const;
         [[nodiscard]] std::size_t index( const std::string& in_name ) const;

         [[nodiscard]] bool empty() const;
         [[nodiscard]] std::size_t size() const;

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
            [[nodiscard]] friend bool operator!=( const const_iterator& lhs, const const_iterator& rhs )
            {
               return lhs.row_ != rhs.row_;
            }

            const_iterator& operator++()
            {
               ++row_;
               return *this;
            }

            [[nodiscard]] const row& operator*() const
            {
               return *this;
            }
         };

         [[nodiscard]] const_iterator begin() const;
         [[nodiscard]] const_iterator end() const;

         [[nodiscard]] bool is_null( const std::size_t row, const std::size_t column ) const;
         [[nodiscard]] const char* get( const std::size_t row, const std::size_t column ) const;

         [[nodiscard]] row operator[]( const std::size_t row ) const
         {
            return postgres::row( *this, row, 0, columns_ );
         }

         [[nodiscard]] row at( const std::size_t row ) const;

         template< typename T >
         [[nodiscard]] T as() const
         {
            if( size() != 1 ) {
               throw std::runtime_error( utility::printf( "invalid result size: %zu rows, expected 1 row", rows_ ) );
            }
            return ( *this )[ 0 ].as< T >();
         }

         template< typename T >
         [[nodiscard]] std::optional< T > optional() const
         {
            if( empty() ) {
               return {};
            }
            return as< T >();
         }

         template< typename T, typename U >
         [[nodiscard]] std::pair< T, U > pair() const
         {
            return as< std::pair< T, U > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::tuple< Ts... > tuple() const
         {
            return as< std::tuple< Ts... > >();
         }

         template< typename T >
         [[nodiscard]] std::enable_if_t< result_impl::has_reserve< T >, T > as_container() const
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
         [[nodiscard]] std::enable_if_t< !result_impl::has_reserve< T >, T > as_container() const
         {
            T nrv;
            check_has_result_set();
            for( const auto& row : *this ) {
               nrv.insert( nrv.end(), row.as< typename T::value_type >() );
            }
            return nrv;
         }

         template< typename... Ts >
         [[nodiscard]] std::vector< Ts... > vector() const
         {
            return as_container< std::vector< Ts... > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::list< Ts... > list() const
         {
            return as_container< std::list< Ts... > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::set< Ts... > set() const
         {
            return as_container< std::set< Ts... > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::multiset< Ts... > multiset() const
         {
            return as_container< std::multiset< Ts... > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::unordered_set< Ts... > unordered_set() const
         {
            return as_container< std::unordered_set< Ts... > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::unordered_multiset< Ts... > unordered_multiset() const
         {
            return as_container< std::unordered_multiset< Ts... > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::map< Ts... > map() const
         {
            return as_container< std::map< Ts... > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::multimap< Ts... > multimap() const
         {
            return as_container< std::multimap< Ts... > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::unordered_map< Ts... > unordered_map() const
         {
            return as_container< std::unordered_map< Ts... > >();
         }

         template< typename... Ts >
         [[nodiscard]] std::unordered_multimap< Ts... > unordered_multimap() const
         {
            return as_container< std::unordered_multimap< Ts... > >();
         }

         // make sure you include libpq-fe.h before accessing the raw pointer
         [[nodiscard]] ::PGresult* underlying_raw_ptr()
         {
            return pgresult_.get();
         }

         // make sure you include libpq-fe.h before accessing the raw pointer
         [[nodiscard]] const ::PGresult* underlying_raw_ptr() const
         {
            return pgresult_.get();
         }
      };

   }  // namespace postgres

}  // namespace tao

#endif
