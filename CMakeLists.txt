cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")


add_definitions(${CMAKE_CXX_FLAGS} -D__STDC_CONSTANT_MACROS)
add_definitions(${CMAKE_CXX_FLAGS} -D__STDC_FORMAT_MACROS)
add_definitions(${CMAKE_CXX_FLAGS} -D__STDC_LIMIT_MACROS)

add_definitions(${CMAKE_CXX_FLAGS} -Wall)
add_definitions(${CMAKE_CXX_FLAGS} -Wextra)
add_definitions(${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden)

project(kscope)

find_package(LLVM)

if( NOT LLVM_FOUND )
  message(FATAL_ERROR "LLVM package can't be found. Set CMAKE_PREFIX_PATH variable to LLVM's installation prefix.")
endif()

include_directories( src ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )

set(CMAKE_BUILD_TYPE Debug)

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit native)

add_subdirectory(src)
