set(PROJECT_NAME EmeraldTests)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
        tokeniserTests.cpp
)

get_target_property(EmeraldLib_INCLUDES EmeraldLib INCLUDE_DIRECTORIES)
target_include_directories(${PROJECT_NAME} PRIVATE ${EmeraldLib_INCLUDES})
target_link_libraries(${PROJECT_NAME}
        PUBLIC EmeraldLib
        PUBLIC gtest_main
        PUBLIC gmock
        PUBLIC gmock_main
        PUBLIC gtest
)