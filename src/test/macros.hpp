// Copyright (c) 2016-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef SRC_TEST_MACROS_HPP  // NOLINT(llvm-header-guard)
#define SRC_TEST_MACROS_HPP

// This is an internal header used for unit-tests.

#include <cstdlib>
#include <iostream>

#include <tao/pq/exception.hpp>
#include <tao/pq/internal/demangle.hpp>

#define STRINGIFY_INTERNAL( ... ) #__VA_ARGS__
#define STRINGIFY( ... ) STRINGIFY_INTERNAL( __VA_ARGS__ )
#define FILE_AND_LINE __FILE__ ":" STRINGIFY( __LINE__ )

#define TEST_EXECUTE_MESSAGE( MeSSaGe, ... )                             \
   do {                                                                  \
      std::cout << "TEST [ " MeSSaGe << " ] in [ " FILE_AND_LINE " ]\n"; \
      __VA_ARGS__;                                                       \
   } while( false )

#define TEST_FAILED                                          \
   do {                                                      \
      std::cerr << "TEST FAILED in [ " FILE_AND_LINE " ]\n"; \
      std::exit( 1 );                                        \
   } while( false )

#define TEST_ASSERT_MESSAGE( MeSSaGe, ... )       \
   TEST_EXECUTE_MESSAGE(                          \
      MeSSaGe,                                    \
      if( !static_cast< bool >( __VA_ARGS__ ) ) { \
         TEST_FAILED;                             \
      } )

#define TEST_THROWS_MESSAGE( MeSSaGe, ... )                                                   \
   TEST_EXECUTE_MESSAGE(                                                                      \
      MeSSaGe,                                                                                \
      try {                                                                                   \
         __VA_ARGS__;                                                                         \
         TEST_FAILED;                                                                         \
      } catch( const tao::pq::sql_error& e ) {                                                \
         std::cout << "TEST caught [ " << tao::pq::internal::demangle( typeid( e ) ) << " ] " \
                   << "with SQLSTATE [ " << e.sqlstate << " ] "                               \
                   << "and [ " << e.what() << " ] in [ " FILE_AND_LINE " ]\n";                \
      } catch( const std::exception& e ) {                                                    \
         std::cout << "TEST caught [ " << tao::pq::internal::demangle( typeid( e ) ) << " ] " \
                   << "with [ " << e.what() << " ] in [ " FILE_AND_LINE " ]\n";               \
      } catch( ... ) {                                                                        \
         std::cout << "TEST caught unknown exception in [ " FILE_AND_LINE " ]\n";             \
      } )

#define TEST_EXECUTE( ... ) TEST_EXECUTE_MESSAGE( "EXECUTE " #__VA_ARGS__, __VA_ARGS__ )
#define TEST_ASSERT( ... ) TEST_ASSERT_MESSAGE( "ASSERT " #__VA_ARGS__, __VA_ARGS__ )
#define TEST_THROWS( ... ) TEST_THROWS_MESSAGE( "THROWS " #__VA_ARGS__, (void)__VA_ARGS__ )

#endif
