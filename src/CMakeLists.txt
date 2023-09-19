set(PROJECT_NAME Emerald)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    Result.hpp
    NodeVisitor.hpp
    Tokeniser.hpp Tokeniser.cpp
    Parser.hpp Parser.cpp
    Generator.hpp Generator.cpp
    Assembler.hpp Assembler.cpp
    Linker.hpp Linker.cpp
)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CPPARGPARSE_INCLUDE_DIRS})