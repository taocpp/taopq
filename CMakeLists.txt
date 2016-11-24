cmake_minimum_required (VERSION 3.2.2)
project (postgres)

# External dependencies
if(APPLE)
    # Homebrew
    link_directories ("/usr/local/lib")
    include_directories ("/usr/local/include")
elseif(WIN32)
    set(POSTGRES_INCLUDE "" CACHE PATH "PostgreSQL include")
    include_directories (${POSTGRES_INCLUDE})
elseif(UNIX)
    # Tested for RHEL7 only
    link_directories ("/usr/pgsql-9.6/lib")
    include_directories ("/usr/pgsql-9.6/include")
endif()

include_directories ("${PROJECT_SOURCE_DIR}/include")

set (SRC_PATH_INC "${PROJECT_SOURCE_DIR}/include/tao")
set (SRC_PATH_INC_POSTGRES "${PROJECT_SOURCE_DIR}/include/tao/postgres")
set (SRC_PATH_INC_OPTIONAL "${PROJECT_SOURCE_DIR}/include/tao/optional")
set (SRC_PATH_INC_SEQ "${PROJECT_SOURCE_DIR}/include/tao/seq")
set (SRC_PATH_INC_UTILITY "${PROJECT_SOURCE_DIR}/include/tao/utility")

set (SRC_PATH_LIB_POSTGRES "${PROJECT_SOURCE_DIR}/src/lib/postgres")
set (SRC_PATH_LIB_UTILITY "${PROJECT_SOURCE_DIR}/src/lib/utility")

set (SOURCES
    ${SRC_PATH_INC_POSTGRES}/connection_pool.hpp
    ${SRC_PATH_INC_POSTGRES}/connection.hpp
    ${SRC_PATH_INC_POSTGRES}/field.hpp
    ${SRC_PATH_INC_POSTGRES}/null.hpp
    ${SRC_PATH_INC_POSTGRES}/parameter_traits.hpp
    ${SRC_PATH_INC_POSTGRES}/result_traits_optional.hpp
    ${SRC_PATH_INC_POSTGRES}/result_traits_pair.hpp
    ${SRC_PATH_INC_POSTGRES}/result_traits_tuple.hpp
    ${SRC_PATH_INC_POSTGRES}/result_traits.hpp
    ${SRC_PATH_INC_POSTGRES}/result.hpp
    ${SRC_PATH_INC_POSTGRES}/row.hpp
    ${SRC_PATH_INC_POSTGRES}/table_writer.hpp
    ${SRC_PATH_INC_POSTGRES}/transaction.hpp

    ${SRC_PATH_INC_OPTIONAL}/optional.hpp

    ${SRC_PATH_INC_SEQ}/config.hpp
    ${SRC_PATH_INC_SEQ}/exclusive_scan.hpp
    ${SRC_PATH_INC_SEQ}/integer_sequence.hpp
    ${SRC_PATH_INC_SEQ}/make_integer_sequence.hpp
    ${SRC_PATH_INC_SEQ}/make_integer_sequence.hpp
    ${SRC_PATH_INC_SEQ}/sum.hpp

    ${SRC_PATH_INC_UTILITY}/demangle.hpp
    ${SRC_PATH_INC_UTILITY}/getenv.hpp
    ${SRC_PATH_INC_UTILITY}/make_unique.hpp
    ${SRC_PATH_INC_UTILITY}/pool.hpp
    ${SRC_PATH_INC_UTILITY}/printf.hpp
    ${SRC_PATH_INC_UTILITY}/strtox.hpp

    ${SRC_PATH_INC}/postgres.hpp

    ${SRC_PATH_LIB_POSTGRES}/connection_pool.cpp
    ${SRC_PATH_LIB_POSTGRES}/connection.cpp
    ${SRC_PATH_LIB_POSTGRES}/field.cpp
    ${SRC_PATH_LIB_POSTGRES}/parameter_traits.cpp
    ${SRC_PATH_LIB_POSTGRES}/result_traits.cpp
    ${SRC_PATH_LIB_POSTGRES}/result.cpp
    ${SRC_PATH_LIB_POSTGRES}/row.cpp
    ${SRC_PATH_LIB_POSTGRES}/table_writer.cpp
    ${SRC_PATH_LIB_POSTGRES}/transaction.cpp

    ${SRC_PATH_LIB_UTILITY}/demangle.cpp
    ${SRC_PATH_LIB_UTILITY}/getenv.cpp
    ${SRC_PATH_LIB_UTILITY}/printf.cpp
    ${SRC_PATH_LIB_UTILITY}/strtox.cpp
)

add_library (postgres ${SOURCES})

target_link_libraries (postgres pq)

set_property(TARGET postgres PROPERTY CXX_STANDARD 11)
