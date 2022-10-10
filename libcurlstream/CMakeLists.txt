cmake_minimum_required (VERSION 3.8)

set(TARGET "libcurlstream")

add_library(${TARGET} INTERFACE)

find_package(CURL REQUIRED)
target_include_directories(${TARGET} INTERFACE ${CURL_INCLUDE_DIRS})
target_link_libraries(${TARGET} INTERFACE ${CURL_LIBRARIES})

if(WIN32)
	target_compile_definitions(${TARGET} INTERFACE "NOMINMAX")
endif()

target_include_directories(${TARGET} INTERFACE "./include")