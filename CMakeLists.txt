cmake_minimum_required(VERSION 3.23)
project(sat-solver LANGUAGES CXX C)
message(STATUS "----- ${PROJECT_NAME}")

include(cmake/modules/SetOutputDirs.cmake)
include(cmake/modules/UTF8Encoding.cmake)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE BENCHMARKS "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/*")

add_subdirectory(src)
