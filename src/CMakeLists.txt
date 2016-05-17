file(GLOB KSCOPE_SOURCES *.cc)

add_subdirectory(Parse)
add_subdirectory(Render)

add_executable(kscope ${KSCOPE_SOURCES})

target_link_libraries(kscope parsing ir_pipeline ir_rendering ${REQ_LLVM_LIBRARIES} -lc++ -lm)
