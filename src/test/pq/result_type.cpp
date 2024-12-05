// Copyright (c) 2023-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <list>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <tao/pq.hpp>

static_assert( !tao::pq::result_type< void > );

static_assert( tao::pq::result_type< bool > );

static_assert( tao::pq::result_type< char > );
static_assert( tao::pq::result_type< signed char > );
static_assert( tao::pq::result_type< unsigned char > );

static_assert( tao::pq::result_type< short > );
static_assert( tao::pq::result_type< unsigned short > );
static_assert( tao::pq::result_type< int > );
static_assert( tao::pq::result_type< unsigned > );
static_assert( tao::pq::result_type< long > );
static_assert( tao::pq::result_type< unsigned long > );
static_assert( tao::pq::result_type< long long > );
static_assert( tao::pq::result_type< unsigned long long > );

static_assert( tao::pq::result_type< float > );
static_assert( tao::pq::result_type< double > );
static_assert( tao::pq::result_type< long double > );

static_assert( tao::pq::result_type< const char* > );
static_assert( tao::pq::result_type< std::string > );
static_assert( tao::pq::result_type< std::string_view > );

static_assert( tao::pq::result_type< tao::pq::binary > );
static_assert( !tao::pq::result_type< tao::pq::binary_view > );

static_assert( !tao::pq::result_type< std::span< std::byte > > );
static_assert( !tao::pq::result_type< std::span< std::byte, 42 > > );
static_assert( !tao::pq::result_type< std::span< const std::byte > > );
static_assert( !tao::pq::result_type< std::span< const std::byte, 42 > > );
static_assert( tao::pq::result_type< std::vector< std::byte > > );

// optional
static_assert( tao::pq::result_type< std::optional< int > > );
static_assert( tao::pq::result_type< std::optional< std::string > > );

// pair
static_assert( tao::pq::result_type< std::pair< bool, int > > );
static_assert( tao::pq::result_type< std::pair< std::string, tao::pq::binary > > );

// tuple
static_assert( !tao::pq::result_type< std::tuple<> > );
static_assert( tao::pq::result_type< std::tuple< int > > );
static_assert( tao::pq::result_type< std::tuple< bool, int, float > > );
static_assert( tao::pq::result_type< std::tuple< std::string, tao::pq::binary, unsigned > > );

// array
static_assert( !tao::pq::result_type< std::array< int, 42 > > );
static_assert( !tao::pq::result_type< std::array< std::string, 42 > > );
static_assert( tao::pq::result_type< std::list< std::string_view > > );
static_assert( tao::pq::result_type< std::set< double > > );
static_assert( tao::pq::result_type< std::unordered_set< char > > );
static_assert( !tao::pq::result_type< std::set< std::pair< int, double > > > );
static_assert( !tao::pq::result_type_direct< std::set< std::tuple<> > > );
static_assert( tao::pq::result_type< std::set< std::tuple< int > > > );
static_assert( !tao::pq::result_type< std::set< std::tuple< bool, int, double > > > );

// note: vector<T> except for T == std::byte are registered as arrays by default
static_assert( tao::pq::result_type< std::vector< bool > > );
static_assert( tao::pq::result_type< std::vector< unsigned long long > > );

static_assert( tao::pq::result_type< std::vector< std::set< double > > > );
static_assert( tao::pq::result_type< std::set< std::vector< double > > > );
static_assert( tao::pq::result_type< std::list< std::unordered_set< std::tuple< int > > > > );

// aggregate
namespace example
{
   struct user
   {
      std::string name;
      int age;
      std::string planet;
   };

   struct user2
   {
      std::string name;
      int age;
      std::string planet;
   };

}  // namespace example

template<>
inline constexpr bool tao::pq::is_aggregate< example::user > = true;

static_assert( tao::pq::result_type< example::user > );
static_assert( !tao::pq::result_type< example::user2 > );  // not registered

auto main() -> int
{
   return 0;
}
