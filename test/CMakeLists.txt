cmake_minimum_required(VERSION 3.15)

option(BUILD_INTEGRATION_TESTS "Build integration tests that require a live PostgreSQL database" ON)
option(BUILD_UNIT_TESTS "Build unit tests without needing a live database" ON)

include(CTest)

set(SOURCE_INTEGRATION_TESTS
  integration/aggregate.cpp
  integration/array.cpp
  integration/basic_datatypes.cpp
  integration/chunk_mode.cpp
  integration/connection.cpp
  integration/connection_pool.cpp
  integration/example.cpp
  integration/exception.cpp
  integration/large_object.cpp
  integration/log.cpp
  integration/notifications.cpp
  integration/parameter.cpp
  integration/password.cpp
  integration/pipeline_mode.cpp
  integration/result.cpp
  integration/row.cpp
  integration/single_row_mode.cpp
  integration/table_reader.cpp
  integration/table_writer.cpp
  integration/traits.cpp
  integration/transaction.cpp
)

set(SOURCE_UNIT_TESTS
  unit/getenv.cpp
  unit/parameter_type.cpp
  unit/resize_uninitialized.cpp
  unit/result_type.cpp
  unit/strtox.cpp
)

function(add_taopq_test source_file)
  get_filename_component(test_name "${source_file}" NAME_WE)
  add_executable("${test_name}" "${source_file}")
  target_link_libraries("${test_name}" PRIVATE taocpp::taopq)
  target_compile_features("${test_name}" PRIVATE cxx_std_20)
  target_include_directories("${test_name}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
  if(WIN32)
    target_link_libraries("${test_name}" PRIVATE wsock32 ws2_32)
  endif()
  add_test(
    NAME "${test_name}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    COMMAND "$<TARGET_FILE:${test_name}>"
  )
endfunction()

foreach(cat IN ITEMS INTEGRATION UNIT)
  if(BUILD_${cat}_TESTS)
    foreach(src IN LISTS SOURCE_${cat}_TESTS)
      add_taopq_test("${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    endforeach()
  endif()
endforeach()
