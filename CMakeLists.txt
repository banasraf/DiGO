cmake_minimum_required(VERSION 3.15)
project(DiGO CXX)

# dependencies
find_package(gRPC CONFIG REQUIRED)
# find_package(modules CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

add_subdirectory(digo)

add_library(DiGO INTERFACE)
target_link_libraries(DiGO INTERFACE DiGO-engine DiGO-opslib)
target_include_directories(DiGO INTERFACE .)
target_include_directories(DiGO INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(examples)