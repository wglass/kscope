cmake_minimum_required(VERSION 2.6)
project(kscope)

add_definitions(${CMAKE_CXX_FLAGS} -D__STDC_CONSTANT_MACROS)
add_definitions(${CMAKE_CXX_FLAGS} -D__STDC_FORMAT_MACROS)
add_definitions(${CMAKE_CXX_FLAGS} -D__STDC_LIMIT_MACROS)

add_definitions(${CMAKE_CXX_FLAGS} -Wall)
#add_definitions(${CMAKE_CXX_FLAGS} -fvisibility)

add_subdirectory(src)
