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
    linker.hpp
    linux_linker.hpp linux_linker.cpp
    windows_linker.hpp windows_linker.cpp
    rules.hpp
    label_manager.hpp
    assembly_builder.hpp
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC EmeraldLib
        PUBLIC sage
)

get_target_property(EmeraldLib_INCLUDES EmeraldLib INCLUDE_DIRECTORIES)
target_include_directories(${PROJECT_NAME}
        PRIVATE ${EmeraldLib_INCLUDES})