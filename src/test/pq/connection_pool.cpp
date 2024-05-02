// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "../getenv.hpp"
#include "../macros.hpp"

#include <tao/pq/connection_pool.hpp>

class limited_connection_pool
   : public tao::pq::connection_pool
{
   struct guard
   {
      std::atomic< std::size_t >& m_counter;

      explicit guard( std::atomic< std::size_t >& counter ) noexcept
         : m_counter( counter )
      {
         ++m_counter;
      }

      guard( const guard& ) = delete;
      guard( guard&& ) = delete;
      void operator=( const guard& ) = delete;
      void operator=( guard&& ) = delete;

      ~guard()
      {
         --m_counter;
      }
   };

   mutable std::atomic< std::size_t > m_creating = 0;

   using tao::pq::connection_pool::connection_pool;

   [[nodiscard]] auto v_create() const -> std::unique_ptr< tao::pq::connection > override
   {
      if( attached() >= 4 || ( m_creating.load() > 2 ) ) {
         throw std::runtime_error( "connection limit reached" );
      }
      const guard g( m_creating );
      return connection_pool::v_create();
   }
};

auto my_poll( const int /*unused*/, const bool /*unused*/, const int /*unused*/ ) -> tao::pq::poll::status
{
   TAO_PQ_UNREACHABLE;
}

void run()
{
   // overwrite the default with an environment variable if needed
   const auto connection_string = tao::pq::internal::getenv( "TAOPQ_TEST_DATABASE", "dbname=template1" );

   const auto pool = tao::pq::connection_pool::create< limited_connection_pool >( connection_string );

   TEST_ASSERT( pool->empty() );
   TEST_ASSERT( pool->attached() == 0 );

   TEST_ASSERT( pool->connection() );

   TEST_ASSERT( !pool->empty() );
   TEST_ASSERT( pool->size() == 1 );
   TEST_ASSERT( pool->attached() == 0 );

   TEST_ASSERT( pool->connection()->execute( "SELECT 1" ).as< int >() == 1 );

   TEST_ASSERT( pool->size() == 1 );
   TEST_ASSERT( pool->attached() == 0 );

   {
      const auto conn = pool->connection();
      TEST_ASSERT( pool->connection() );
      TEST_ASSERT( conn->execute( "SELECT 2" ).as< int >() == 2 );

      TEST_ASSERT( pool->size() == 1 );
      TEST_ASSERT( pool->attached() == 1 );

      const auto pool2 = tao::pq::connection_pool::create( connection_string );
      TEST_ASSERT( pool->connection()->execute( "SELECT 3" ).as< int >() == 3 );
      TEST_ASSERT( pool2->connection()->execute( "SELECT 4" ).as< int >() == 4 );
      TEST_ASSERT( conn->execute( "SELECT 5" ).as< int >() == 5 );
      TEST_ASSERT( pool2->connection()->execute( "SELECT 6" ).as< int >() == 6 );
   }

   TEST_ASSERT( pool->size() == 2 );
   TEST_ASSERT( pool->attached() == 0 );
   {
      [[maybe_unused]] const auto c0 = pool->connection();
      [[maybe_unused]] const auto c1 = pool->connection();
      TEST_ASSERT( pool->empty() );
      TEST_ASSERT( pool->attached() == 2 );
      {
         [[maybe_unused]] const auto c2 = pool->connection();
         [[maybe_unused]] const auto c3 = pool->connection();
         TEST_ASSERT( pool->empty() );
         TEST_ASSERT( pool->attached() == 4 );

         TEST_THROWS( pool->connection() );

         TEST_ASSERT( pool->empty() );
         TEST_ASSERT( pool->attached() == 4 );
      }
      TEST_ASSERT( pool->size() == 2 );
      TEST_ASSERT( pool->attached() == 2 );
   }
   TEST_ASSERT( pool->size() == 4 );
   TEST_ASSERT( pool->attached() == 0 );

   {
      using callback_t = tao::pq::poll::status ( * )( int, bool, int );

      const auto old_cb = *pool->poll_callback().target< callback_t >();
      TEST_ASSERT( old_cb != nullptr );
      TEST_ASSERT( *pool->poll_callback().target< callback_t >() != &my_poll );
      TEST_ASSERT( *pool->connection()->poll_callback().target< callback_t >() != &my_poll );
      pool->set_poll_callback( my_poll );
      TEST_ASSERT( *pool->poll_callback().target< callback_t >() == &my_poll );
      TEST_ASSERT( *pool->connection()->poll_callback().target< callback_t >() == &my_poll );
      pool->reset_poll_callback();
      TEST_ASSERT( *pool->poll_callback().target< callback_t >() == old_cb );
      TEST_ASSERT( *pool->connection()->poll_callback().target< callback_t >() == old_cb );
   }

   using namespace std::chrono_literals;
   pool->set_timeout( 100ms );
   TEST_THROWS( pool->execute( "SELECT pg_sleep( .5 )" ) );

   pool->reset_timeout();
   TEST_EXECUTE( pool->execute( "SELECT pg_sleep( .5 )" ) );
}

auto main() -> int  // NOLINT(bugprone-exception-escape)
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
