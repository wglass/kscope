find_package(LLVM)

if( NOT LLVM_FOUND )
  message(FATAL_ERROR "LLVM package can't be found. Set CMAKE_PREFIX_PATH variable to LLVM's installation prefix.")
endif()

set(KSCOPE_SOURCES
        lexer.cpp
        parser.cpp
        codegen.cpp
        errors.cpp
        main.cpp
)

include_directories( ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit native)

add_executable(kscope ${KSCOPE_SOURCES})

target_link_libraries(kscope ${REQ_LLVM_LIBRARIES})
