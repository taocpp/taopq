file(GLOB testsources *.cpp)
foreach(testsourcefile ${testsources})
  get_filename_component(exename taopq-test-${testsourcefile} NAME_WE)
  add_executable(${exename} ${testsourcefile})
  target_include_directories(taopq PRIVATE ${PostgreSQL_INCLUDE_DIRS})
  target_link_libraries(${exename} PRIVATE taocpp::taopq)
  set_target_properties(${exename} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )
  if(MSVC)
    target_compile_options(${exename} PRIVATE /W4 /WX /utf-8)
  else()
    target_compile_options(${exename} PRIVATE -pedantic -Wall -Wextra -Wshadow -Werror)
  endif()
  if(WIN32)
    target_link_libraries(${exename} PRIVATE wsock32 ws2_32)
  endif()
  add_test(NAME ${exename} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../.. COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${exename})
endforeach(testsourcefile)
