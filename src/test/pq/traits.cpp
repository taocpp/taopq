// Copyright (c) 2020-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq.hpp>

namespace example
{
   struct user
   {
      int a, b, c, d;

      explicit user( const int i ) noexcept
         : a( i ), b( i + 1 ), c( i + 2 ), d( i + 3 )
      {}

      [[nodiscard]] auto to_taopq() const noexcept
      {
         return std::tie( a, b, c, d );
      }

   private:
      user( const int in_a, const int in_b, const int in_c, const int in_d ) noexcept
         : a( in_a ), b( in_b ), c( in_c ), d( in_d )
      {}

   public:
      [[nodiscard]] static auto from_taopq( const int in_a, const int in_b, const int in_c, const int in_d ) noexcept
      {
         return user( in_a, in_b, in_c, in_d );
      }
   };

   struct user2
   {
      int a, b, c, d;

      explicit user2( int i ) noexcept
         : a( i ), b( i + 1 ), c( i + 2 ), d( i + 3 )
      {}

      user2( const int in_a, const int in_b, const int in_c, const int in_d ) noexcept
         : a( in_a ), b( in_b ), c( in_c ), d( in_d )
      {}
   };

   struct user3
   {
      int a, b, c, d;

      explicit user3( int i ) noexcept
         : a( i ), b( i + 1 ), c( i + 2 ), d( i + 3 )
      {}

      user3( const int in_a, const int in_b, const int in_c, const int in_d ) noexcept
         : a( in_a ), b( in_b ), c( in_c ), d( in_d )
      {}
   };

   [[nodiscard]] auto to_taopq( const user3& v ) noexcept
   {
      return std::tie( v.a, v.b, v.c, v.d );
   }

}  // namespace example

template<>
struct tao::pq::bind< example::user2 >
{
   [[nodiscard]] static auto to_taopq( const example::user2& v ) noexcept
   {
      return std::tie( v.a, v.b, v.c, v.d );
   }

   [[nodiscard]] static auto from_taopq( const int a, const int b, const int c, const int d ) noexcept
   {
      return example::user2( a, b, c, d );
   }
};

void run()
{
   const auto connection = tao::pq::connection::create( tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" ) );

   connection->execute( "DROP TABLE IF EXISTS tao_traits_test" );
   connection->execute( "CREATE TABLE tao_traits_test ( a INTEGER PRIMARY KEY, b INTEGER, c INTEGER, d INTEGER )" );

   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( 1, 2, 3, 4 )" ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( $1, $2, $3, $4 )", 2, 3, 4, 5 ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( $1, $2, $3, $4 )", 3, std::make_pair( 4, 5 ), 6 ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( $1, $2, $3, $4 )", std::make_tuple( 4, 5 ), std::make_tuple( 6, 7 ) ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( $1, $2, $3, $4 )", std::make_tuple( 5, std::make_pair( 6, 7 ), 8 ) ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( $1, $2, $3, $4 )", example::user( 6 ) ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( $1, $2, $3, $4 )", example::user2( 7 ) ) );

   const auto result = connection->execute( "SELECT * FROM tao_traits_test" );
   TEST_ASSERT( result.size() == 7 );

   for( const auto& row : result ) {
      {
         const auto v = row.tuple< int, int, int, int >();
         TEST_ASSERT( std::get< 1 >( v ) == std::get< 0 >( v ) + 1 );
         TEST_ASSERT( std::get< 2 >( v ) == std::get< 1 >( v ) + 1 );
         TEST_ASSERT( std::get< 3 >( v ) == std::get< 2 >( v ) + 1 );
      }
      {
         const auto v = row.tuple< std::tuple< int, int >, std::tuple< int, int > >();
         TEST_ASSERT( std::get< 1 >( std::get< 0 >( v ) ) == std::get< 0 >( std::get< 0 >( v ) ) + 1 );
         TEST_ASSERT( std::get< 0 >( std::get< 1 >( v ) ) == std::get< 1 >( std::get< 0 >( v ) ) + 1 );
         TEST_ASSERT( std::get< 1 >( std::get< 1 >( v ) ) == std::get< 0 >( std::get< 1 >( v ) ) + 1 );
      }
      {
         const auto v = row.tuple< int, std::tuple< int, int >, int >();
         TEST_ASSERT( std::get< 0 >( std::get< 1 >( v ) ) == std::get< 0 >( v ) + 1 );
         TEST_ASSERT( std::get< 1 >( std::get< 1 >( v ) ) == std::get< 0 >( std::get< 1 >( v ) ) + 1 );
         TEST_ASSERT( std::get< 2 >( v ) == std::get< 1 >( std::get< 1 >( v ) ) + 1 );
      }
      {
         const auto [ a, b, c, d ] = row.tuple< int, int, int, int >();
         TEST_ASSERT( b == a + 1 );
         TEST_ASSERT( c == b + 1 );
         TEST_ASSERT( d == c + 1 );
      }
   }

   TEST_EXECUTE( connection->execute( "DELETE FROM tao_traits_test" ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( $1, $2, $3, $4 )", example::user( 8 ) ) );
   {
      const auto user = connection->execute( "SELECT * FROM tao_traits_test" ).as< example::user >();
      TEST_ASSERT( user.a == 8 );
      TEST_ASSERT( user.b == 9 );
      TEST_ASSERT( user.c == 10 );
      TEST_ASSERT( user.d == 11 );
   }

   TEST_EXECUTE( connection->execute( "DELETE FROM tao_traits_test" ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( $1, $2, $3, $4 )", example::user2( 9 ) ) );
   {
      const auto user = connection->execute( "SELECT * FROM tao_traits_test" ).as< example::user2 >();
      TEST_ASSERT( user.a == 9 );
      TEST_ASSERT( user.b == 10 );
      TEST_ASSERT( user.c == 11 );
      TEST_ASSERT( user.d == 12 );
   }

   TEST_EXECUTE( connection->execute( "DELETE FROM tao_traits_test" ) );
   TEST_EXECUTE( connection->execute( "INSERT INTO tao_traits_test VALUES ( $1, $2, $3, $4 )", example::user3( 10 ) ) );
   {
      const auto user = connection->execute( "SELECT * FROM tao_traits_test" ).as< example::user >();
      TEST_ASSERT( user.a == 10 );
      TEST_ASSERT( user.b == 11 );
      TEST_ASSERT( user.c == 12 );
      TEST_ASSERT( user.d == 13 );
   }
}

auto main() -> int  //NOLINT(bugprone-exception-escape)
{
   try {
      run();
   }
   // LCOV_EXCL_START
   catch( const std::exception& e ) {
      std::cerr << "exception: " << e.what() << std::endl;
      throw;
   }
   catch( ... ) {
      std::cerr << "unknown exception" << std::endl;
      throw;
   }
   // LCOV_EXCL_STOP
}
