cmake_minimum_required(VERSION 2.6)
project(cpp_external_processes CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-g")

add_subdirectory(lib)

add_executable(run_serial_prime serial_prime.cpp)
add_executable(run_chat chat.cpp)
add_executable(run_calc calc.cpp)

include_directories(
	PUBLIC ${PROJECT_SOURCE_DIR}/lib/
)

target_link_libraries (run_serial_prime ict_exp3_parser)
target_link_libraries (run_chat ict_exp3_parser)
target_link_libraries (run_calc ict_exp3_parser)

