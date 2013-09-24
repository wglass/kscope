file(GLOB KSCOPE_SOURCES *.cpp)

add_subdirectory(ast)

add_executable(kscope ${KSCOPE_SOURCES})

target_link_libraries(kscope ast ${REQ_LLVM_LIBRARIES})
