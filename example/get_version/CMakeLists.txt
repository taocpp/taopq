cmake_minimum_required(VERSION 3.15)
project(taopq_example_get_version CXX)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE taocpp::taopq)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20 c_std_11)
