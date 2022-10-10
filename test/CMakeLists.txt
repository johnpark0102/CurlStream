cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(TARGET "test_csb")
add_executable(${TARGET} "test_main.cpp")

target_link_libraries(${TARGET} PRIVATE "libcurlstream")