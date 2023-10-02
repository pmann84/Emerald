set(PROJECT_NAME EmeraldLib)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PRIVATE
        Token.hpp Token.cpp
        Tokeniser.hpp Tokeniser.cpp
        CompilerError.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})