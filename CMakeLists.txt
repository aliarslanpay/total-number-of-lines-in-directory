cmake_minimum_required(VERSION 3.25)
project(total_number_of_lines_in_directory)

add_executable(total_number_of_lines_in_directory main.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
