cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++")


add_definitions(${CMAKE_CXX_FLAGS} -D__STDC_CONSTANT_MACROS)
add_definitions(${CMAKE_CXX_FLAGS} -D__STDC_FORMAT_MACROS)
add_definitions(${CMAKE_CXX_FLAGS} -D__STDC_LIMIT_MACROS)

add_definitions(${CMAKE_CXX_FLAGS} -Wall)
add_definitions(${CMAKE_CXX_FLAGS} -Wextra)
add_definitions(${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden)
add_definitions(${CMAKE_CXX_FLAGS} -Wno-unused-parameter)

project(kscope)

find_package(LLVM REQUIRED CONFIG)

include_directories( include src ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )

set(CMAKE_BUILD_TYPE Debug)

llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES core support mcjit native orcjit)

add_subdirectory(src)
