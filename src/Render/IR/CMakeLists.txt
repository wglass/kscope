file(GLOB IR_RENDERING_SOURCES *.cc)

add_subdirectory(Pipeline)

add_library(ir_rendering ${IR_RENDERING_SOURCES})
