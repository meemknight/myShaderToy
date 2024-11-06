cmake_minimum_required(VERSION 3.1)
project(escapi)

add_library(escapi)
target_sources(escapi PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/capture.cpp "
	"${CMAKE_CURRENT_SOURCE_DIR}/src/conversion.cpp "
	"${CMAKE_CURRENT_SOURCE_DIR}/src/escapi.cpp "
	"${CMAKE_CURRENT_SOURCE_DIR}/src/escapi_dll.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/interface.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/videobufferlock.cpp"

)
target_include_directories(escapi PUBLIC include)