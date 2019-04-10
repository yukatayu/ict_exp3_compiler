cmake_minimum_required(VERSION 2.6)
project(cpp_external_processes CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ict_exp3_parser helper.cpp data.cpp parser.cpp)

include_directories(
	PUBLIC ${PROJECT_SOURCE_DIR}/
)

