set(PROJECT_NAME Emerald)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    NodeVisitor.hpp
    Variable.hpp
    ArenaAllocator.hpp
    Nodes.hpp
    Parser.hpp Parser.cpp
    Generator.hpp Generator.cpp
    Assembler.hpp Assembler.cpp
    Linker.hpp Linker.cpp
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC EmeraldLib
)

get_target_property(EmeraldLib_INCLUDES EmeraldLib INCLUDE_DIRECTORIES)
target_include_directories(${PROJECT_NAME}
        PRIVATE ${EmeraldLib_INCLUDES}
        PRIVATE ${CPPARGPARSE_INCLUDE_DIRS})