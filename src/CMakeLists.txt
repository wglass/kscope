file(GLOB KSCOPE_SOURCES *.cc)

add_subdirectory(ast)
add_subdirectory(codegen)
add_subdirectory(parsing)

add_executable(kscope ${KSCOPE_SOURCES})

target_link_libraries(kscope ast codegen parsing ${REQ_LLVM_LIBRARIES} -lc++)
