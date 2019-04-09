cmake_minimum_required(VERSION 2.6)
project(cpp_external_processes CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-g")

add_subdirectory(lib)

add_executable(run_02 main_02.cpp)
add_executable(run_04 main_04.cpp)

include_directories(
	PUBLIC ${PROJECT_SOURCE_DIR}/lib/
)

target_link_libraries (run_02 ict_exp3_parser)
target_link_libraries (run_04 ict_exp3_parser)

