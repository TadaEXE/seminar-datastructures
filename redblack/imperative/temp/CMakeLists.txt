cmake_minimum_required(VERSION 3.20)
project(rbt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(rbt
  ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)

target_include_directories(rbt
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
)
