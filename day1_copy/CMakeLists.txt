cmake_minimum_required(VERSION 3.10)
project(day1_copy CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(test1
    src/test1.cpp
    src/min.cpp
)

target_include_directories(test1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)