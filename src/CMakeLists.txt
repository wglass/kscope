set(KSCOPE_SOURCES
        lexer.cpp
        parser.cpp
        codegen.cpp
        errors.cpp
        main.cpp
)

add_subdirectory(ast)


add_executable(kscope ${KSCOPE_SOURCES})

target_link_libraries(kscope ast ${REQ_LLVM_LIBRARIES})
