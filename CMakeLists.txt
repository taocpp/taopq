cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)

# Read version from version.hpp
file(READ "${CMAKE_CURRENT_LIST_DIR}/include/tao/pq/version.hpp" taopq_VERSION_DATA)
string(REGEX MATCH "#define TAO_PQ_VERSION \"([^\"]+)\"" _ ${taopq_VERSION_DATA})
set(taopq_VERSION "${CMAKE_MATCH_1}")

project(taopq VERSION ${taopq_VERSION} LANGUAGES CXX)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

find_package(PostgreSQL REQUIRED)

set(taopq_INSTALL_INCLUDE_DIR "include" CACHE STRING "The installation include directory")
set(taopq_INSTALL_DOC_DIR "share/doc/tao/pq" CACHE STRING "The installation doc directory")
option(taopq_BUILD_TESTS "Build test programs" ON)

set(taopq_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)

set(taopq_INCLUDE_FILES
  ${taopq_INCLUDE_DIRS}/tao/pq.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/access_mode.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/binary.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/bind.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/connection.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/connection_pool.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/exception.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/field.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/aggregate.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/demangle.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/dependent_false.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/exclusive_scan.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/from_chars.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/gen.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/parameter_traits_helper.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/poll.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/pool.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/printf.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/resize_uninitialized.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/strtox.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/unreachable.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/internal/zsv.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/is_aggregate.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/isolation_level.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/large_object.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/notification.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/null.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/oid.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/parameter.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/parameter_traits.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/parameter_traits_aggregate.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/parameter_traits_array.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/parameter_traits_optional.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/parameter_traits_pair.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/parameter_traits_tuple.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/poll.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/result.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/result_traits.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/result_traits_aggregate.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/result_traits_array.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/result_traits_optional.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/result_traits_pair.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/result_traits_tuple.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/row.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/table_field.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/table_reader.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/table_row.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/table_writer.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/transaction.hpp
  ${taopq_INCLUDE_DIRS}/tao/pq/version.hpp
)

set(taopq_SOURCE_FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/connection.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/connection_pool.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/exception.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/field.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/internal/demangle.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/internal/poll.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/internal/printf.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/internal/strtox.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/large_object.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/parameter_traits.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/result.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/result_traits.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/row.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/table_field.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/table_reader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/table_row.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/table_writer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/pq/transaction.cpp
)

source_group("Header Files" FILES ${taopq_INCLUDE_FILES})

add_library(taopq ${taopq_SOURCE_FILES} ${taopq_INCLUDE_FILES})
add_library(taocpp::taopq ALIAS taopq)
set_target_properties(taopq PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

target_include_directories(taopq
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${PostgreSQL_INCLUDE_DIRS}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(taopq PUBLIC ${PostgreSQL_LIBRARIES})
if(WIN32)
  target_link_libraries(taopq PUBLIC ws2_32)
endif()

target_compile_features(taopq PUBLIC cxx_std_17)

include(GNUInstallDirs)
set(taopq_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} CACHE STRING "The installation cmake directory")

install(TARGETS taopq EXPORT taopq-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${taopq_INSTALL_INCLUDE_DIR})
install(FILES LICENSE_1_0.txt DESTINATION ${taopq_INSTALL_DOC_DIR})

install(EXPORT taopq-targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE taocpp::
  DESTINATION ${taopq_INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${taopq_INSTALL_CONFIGDIR}
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${taopq_INSTALL_CONFIGDIR}
)

export(EXPORT taopq-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake NAMESPACE taocpp::)
export(PACKAGE taopq)

if(taopq_BUILD_TESTS)
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/test/pq)
endif()
