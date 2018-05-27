cmake_minimum_required (VERSION 3.2.0 FATAL_ERROR)

project (taocpp-postgres VERSION 1.0.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# taocpp/postgres
include ("taocpp-postgres.cmake")
include_directories (${TAOCPP_POSTGRES_INCLUDE_DIRS})

# testing
enable_testing ()
option (TAOCPP_POSTGRES_BUILD_TESTS "Build test programs" ON)
if (TAOCPP_POSTGRES_BUILD_TESTS)
  add_subdirectory (src/test/utility)
  add_subdirectory (src/test/postgres)
endif ()

# installation directories
set (TAOCPP_POSTGRES_INSTALL_INCLUDE_DIR "include" CACHE STRING "The installation include directory")
set (TAOCPP_POSTGRES_INSTALL_DOC_DIR "share/doc/tao/postgres" CACHE STRING "The installation doc directory")

# install
install (DIRECTORY include/ DESTINATION ${TAOCPP_POSTGRES_INSTALL_INCLUDE_DIR})
install (FILES LICENSE DESTINATION ${TAOCPP_POSTGRES_INSTALL_DOC_DIR})
