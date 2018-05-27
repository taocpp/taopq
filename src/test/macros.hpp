// The Art of C++ / PostgreSQL
// Copyright (c) 2016-2018 Daniel Frey

#ifndef SRC_TEST_MACROS_HPP
#define SRC_TEST_MACROS_HPP

// This is an internal header used for unit-tests.

#include <cstdlib>
#include <iostream>

#define STRINGIFY_INTERNAL( ... ) #__VA_ARGS__
#define STRINGIFY( ... ) STRINGIFY_INTERNAL( __VA_ARGS__ )
#define FILE_AND_LINE __FILE__ ":" STRINGIFY( __LINE__ )

#define TEST_EXECUTE_MESSAGE( MESSAGE, ... )                                     \
   std::cout << "TEST [ " MESSAGE << " ] in [ " FILE_AND_LINE " ]" << std::endl; \
   do {                                                                          \
      __VA_ARGS__;                                                               \
   } while( false )

#define TEST_FAILED                                                     \
   do {                                                                 \
      std::cerr << "TEST FAILED in [ " FILE_AND_LINE " ]" << std::endl; \
      std::exit( 1 );                                                   \
   } while( false )

#define TEST_ASSERT_MESSAGE( MESSAGE, ... )                          \
   TEST_EXECUTE_MESSAGE( MESSAGE,                                    \
                         if( !static_cast< bool >( __VA_ARGS__ ) ) { \
                            TEST_FAILED;                             \
                         } )

#define TEST_THROWS_MESSAGE( MESSAGE, ... )                                                                          \
   TEST_EXECUTE_MESSAGE( MESSAGE,                                                                                    \
                         try {                                                                                       \
                            __VA_ARGS__;                                                                             \
                            TEST_FAILED;                                                                             \
                         } catch( const std::exception& e ) {                                                        \
                            std::cout << "TEST caught [ " << e.what() << " ] in [ " FILE_AND_LINE " ]" << std::endl; \
                         } catch( ... ) {                                                                            \
                            std::cout << "TEST caught unknown exception in [ " FILE_AND_LINE " ]" << std::endl;      \
                         } )

#define TEST_EXECUTE( ... ) TEST_EXECUTE_MESSAGE( "EXECUTE " #__VA_ARGS__, __VA_ARGS__ )
#define TEST_ASSERT( ... ) TEST_ASSERT_MESSAGE( "ASSERT " #__VA_ARGS__, __VA_ARGS__ )
#define TEST_THROWS( ... ) TEST_THROWS_MESSAGE( "THROWS " #__VA_ARGS__, __VA_ARGS__ )

#endif
