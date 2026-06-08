cmake_minimum_required(VERSION 3.5)
project(avl_imperative)
set(CMAKE_CXX_STANDARD 17)
add_executable(avl src/main.cpp src/avl.cpp)
add_executable(avl_bench
    src/bench.cpp
    src/avl.cpp
    src/avl_iterative.cpp
    src/avl_compact.cpp
    src/avl_vector.cpp
    src/bench_helpers.cpp)
