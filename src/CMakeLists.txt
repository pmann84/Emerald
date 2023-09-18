set(PROJECT_NAME Emerald)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE main.cpp
)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CPPARGPARSE_INCLUDE_DIRS})