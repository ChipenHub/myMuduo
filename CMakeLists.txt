cmake_minimum_required(VERSION 2.5)
project(mymuduo)

set(CMAKE_CXX_STANDARD 11)
# put the product into root directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# set debug data, use c++11 std
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

# define source files involved compile
aux_source_directory(. SRC_LIST)
# compile .so lib mymuduo
add_library(mymuduo SHARED ${SRC_LIST})
