cmake_minimum_required (VERSION 3.2.0 FATAL_ERROR)

project (taocpp-postgres VERSION 1.0.0 LANGUAGES CXX)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup()
endif()

list (INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

find_package (PostgreSQL REQUIRED)

# taocpp postgres options
set (TAOCPP_POSTGRES_INSTALL_INCLUDE_DIR "include" CACHE STRING "The installation include directory")
set (TAOCPP_POSTGRES_INSTALL_DOC_DIR "share/doc/tao/postgres" CACHE STRING "The installation doc directory")
option (TAOCPP_POSTGRES_BUILD_TESTS "Build test programs" ON)

# taocpp/postgres
set (TAOCPP_POSTGRES_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)

set (TAOCPP_POSTGRES_INCLUDE_FILES
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/table_writer.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/connection_pool.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/null.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/transaction.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/field.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/result.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/row.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/result_traits.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/result_traits_tuple.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/result_traits_optional.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/parameter_traits.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/result_traits_pair.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres/connection.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/utility/getenv.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/utility/strtox.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/utility/demangle.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/utility/printf.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/utility/make_unique.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/utility/pool.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/seq/sum.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/seq/config.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/seq/integer_sequence.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/seq/exclusive_scan.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/seq/make_integer_sequence.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/seq/partial_sum.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/postgres.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/optional/akrzemi1/optional.hpp
  ${TAOCPP_POSTGRES_INCLUDE_DIRS}/tao/optional/optional.hpp
)

set (TAOCPP_POSTGRES_SOURCE_FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/postgres/transaction.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/postgres/result.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/postgres/row.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/postgres/connection.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/postgres/table_writer.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/postgres/connection_pool.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/postgres/result_traits.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/postgres/field.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/utility/strtox.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/utility/getenv.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/utility/printf.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/lib/utility/demangle.cpp
)

source_group ("Header Files" FILES ${TAOCPP_POSTGRES_INCLUDE_FILES})

add_library (postgres ${TAOCPP_POSTGRES_SOURCE_FILES} ${TAOCPP_POSTGRES_INCLUDE_FILES})
add_library (taocpp::postgres ALIAS postgres)
set_target_properties(postgres PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

target_include_directories (postgres
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries (postgres PUBLIC ${PostgreSQL_LIBRARIES})

# features used by the taocpp/postgres
target_compile_features (postgres PRIVATE
  # TODO: Update to the real list...
  cxx_alias_templates
  cxx_auto_type
  cxx_constexpr
  cxx_decltype
  cxx_default_function_template_args
  cxx_defaulted_functions
  cxx_delegating_constructors
  cxx_deleted_functions
  cxx_explicit_conversions
  cxx_generalized_initializers
  cxx_inheriting_constructors
  cxx_inline_namespaces
  cxx_noexcept
  cxx_nonstatic_member_init
  cxx_nullptr
  cxx_range_for
  cxx_rvalue_references
  cxx_static_assert
  cxx_strong_enums
  cxx_template_template_parameters
  cxx_trailing_return_types
  cxx_uniform_initialization
  cxx_variadic_macros
  cxx_variadic_templates
)

# install
include(GNUInstallDirs)
set(TAOCPP_POSTGRES_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} CACHE STRING "The installation cmake directory")

install (TARGETS postgres EXPORT postgres-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install (DIRECTORY include/ DESTINATION ${TAOCPP_POSTGRES_INSTALL_INCLUDE_DIR})
install (FILES LICENSE DESTINATION ${TAOCPP_POSTGRES_INSTALL_DOC_DIR})

install(EXPORT postgres-targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE taocpp::
  DESTINATION ${TAOCPP_POSTGRES_INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${TAOCPP_POSTGRES_INSTALL_CONFIGDIR}
)

# Install the config, configversion and custom find modules
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${TAOCPP_POSTGRES_INSTALL_CONFIGDIR}
)

export(EXPORT postgres-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake NAMESPACE taocpp::)
export(PACKAGE postgres)

# testing
enable_testing ()
if (TAOCPP_POSTGRES_BUILD_TESTS)
  add_subdirectory (${CMAKE_CURRENT_LIST_DIR}/src/test/utility)
  add_subdirectory (${CMAKE_CURRENT_LIST_DIR}/src/test/postgres)
endif ()
