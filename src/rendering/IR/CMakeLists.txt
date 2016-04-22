file(GLOB IR_RENDERING_SOURCES *.cc)

add_subdirectory(ORCPipeline)

add_library(ir_rendering ${IR_RENDERING_SOURCES})
