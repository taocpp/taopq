cmake_minimum_required(VERSION 3.15)

add_subdirectory(get_version)

set(EXAMPLE_TARGETS
  taopq_example_get_version
)

add_custom_target(examples
  COMMENT "Run all examples"
  DEPENDS ${EXAMPLE_TARGETS}
)

foreach(tgt IN LISTS EXAMPLE_TARGETS)
  add_custom_command(TARGET examples POST_BUILD
    COMMAND "$<TARGET_FILE:${tgt}>"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Running ${tgt}"
    VERBATIM
  )
endforeach()