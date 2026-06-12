cmake_minimum_required(VERSION 3.5)
project(rbt_imperative)

set(CMAKE_CXX_STANDARD 17)

add_executable(rbt src/main.cpp src/rbt.cpp)
add_executable(bench src/bench.cpp src/bench_helper.cpp src/rbt.cpp)

target_compile_options(bench PRIVATE -g -O0)

target_include_directories(rbt
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
)