cmake_minimum_required(VERSION 3.15)

# Read version from version.hpp
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/version.hpp" taopq_VERSION_DATA)
string(REGEX MATCH "#define TAO_PQ_VERSION \"([^\"]+)\"" _ ${taopq_VERSION_DATA})
set(taopq_VERSION "${CMAKE_MATCH_1}")

project(taopq
  VERSION ${taopq_VERSION}
  LANGUAGES CXX
  DESCRIPTION "A lightweight C++ client library for accessing a PostgreSQL database"
  HOMEPAGE_URL "https://github.com/taocpp/taopq"
)

find_package(PostgreSQL REQUIRED CONFIG)

add_library(${PROJECT_NAME})
add_library(taocpp::taopq ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/connection.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/connection_pool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/exception.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/field.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/internal/demangle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/internal/poll.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/internal/strtox.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/large_object.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/parameter_traits.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/pipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/result.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/result_traits.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/result_traits_array.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/row.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/table_field.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/table_reader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/table_row.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/table_writer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/transaction.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/pq/transaction_base.cpp
  PUBLIC FILE_SET HEADERS
  BASE_DIRS include
  FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/access_mode.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/binary.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/bind.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/connection.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/connection_pool.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/connection_status.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/exception.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/field.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/aggregate.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/demangle.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/exclusive_scan.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/format_as.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/from_chars.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/gen.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/parameter_traits_helper.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/poll.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/pool.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/resize_uninitialized.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/strtox.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/unreachable.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/internal/zsv.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/is_aggregate.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/is_array.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/isolation_level.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/large_object.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/log.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/notification.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/null.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/oid.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/parameter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/parameter_traits.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/parameter_traits_aggregate.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/parameter_traits_array.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/parameter_traits_optional.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/parameter_traits_pair.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/parameter_traits_tuple.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/pipeline.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/pipeline_status.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/poll.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/result.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/result_status.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/result_traits.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/result_traits_aggregate.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/result_traits_array.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/result_traits_optional.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/result_traits_pair.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/result_traits_tuple.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/row.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/table_field.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/table_reader.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/table_row.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/table_writer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/transaction.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/transaction_base.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/transaction_status.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/tao/pq/version.hpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PUBLIC PostgreSQL::PostgreSQL)
if(WIN32)
  target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

set_target_properties(${PROJECT_NAME} PROPERTIES
  OUTPUT_NAME taopq
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  EXPORT_NAME taopq
)

if(PROJECT_IS_TOP_LEVEL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(TARGETS taopq
    EXPORT taopq-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET HEADERS
  )

  install(FILES LICENSE_1_0.txt DESTINATION ${CMAKE_INSTALL_DOCDIR})

  install(EXPORT taopq-targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE taocpp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/taopq
  )

  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
  )

  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
"include(CMakeFindDependencyMacro)
find_dependency(PostgreSQL REQUIRED CONFIG)
include(\"\${CMAKE_CURRENT_LIST_DIR}/taopqTargets.cmake\")
")

  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/taopq
  )

  export(EXPORT taopq-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
    NAMESPACE taocpp::
  )
endif()

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
  enable_testing()
  add_subdirectory(test/pq)
endif()