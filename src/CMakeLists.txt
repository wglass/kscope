file(GLOB KSCOPE_SOURCES *.cpp)

add_subdirectory(ast)
add_subdirectory(codegen)

add_executable(kscope ${KSCOPE_SOURCES})

target_link_libraries(kscope ast codegen ${REQ_LLVM_LIBRARIES})
