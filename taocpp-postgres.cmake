set (TAOCPP_POSTGRES_LIBRARIES taocpp-postgres)
set (TAOCPP_POSTGRES_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)

file (GLOB_RECURSE TAOCPP_POSTGRES_INCLUDE_FILES ${TAOCPP_POSTGRES_INCLUDE_DIRS}/*.hpp)
file (GLOB_RECURSE TAOCPP_POSTGRES_SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/src/lib/*.cpp)

source_group ("Header Files" FILES ${TAOCPP_POSTGRES_INCLUDE_FILES})

add_library (${TAOCPP_POSTGRES_LIBRARIES} ${TAOCPP_POSTGRES_SOURCE_FILES} ${TAOCPP_POSTGRES_INCLUDE_FILES})
add_library (taocpp::postgres ALIAS ${TAOCPP_POSTGRES_LIBRARIES})
target_link_libraries (${TAOCPP_POSTGRES_LIBRARIES} ${PostgreSQL_LIBRARIES})

# features used by the taocpp/postgres
target_compile_features (${TAOCPP_POSTGRES_LIBRARIES} INTERFACE
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
