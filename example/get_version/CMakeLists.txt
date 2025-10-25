cmake_minimum_required(VERSION 3.15)
project(taopq_example_get_version CXX)

find_package(taopq REQUIRED CONFIG)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE taocpp::taopq)
