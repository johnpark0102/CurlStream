cmake_minimum_required (VERSION 3.8)

project("curlstreambuf")

add_subdirectory("libcurlstream")

if(ENABLE_TEST)
	add_subdirectory("test")
endif()
