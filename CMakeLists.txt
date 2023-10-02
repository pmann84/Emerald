cmake_minimum_required(VERSION 3.22)

# Project settings
project(
        ProjectEmerald
        VERSION 0.0.0.1
        DESCRIPTION ""
        LANGUAGES CXX C
)

# CMake scripts
include(cmake/project_defaults.cmake) # Default project settings

option(PROJECTEMERALD_ENABLE_TESTS "Set to ON to enable the PROJECTEMERALD tests" OFF)

add_subdirectory(vendor)
add_subdirectory(lib)
add_subdirectory(src)

if (PROJECTEMERALD_ENABLE_TESTS)
    add_subdirectory(tests)
endif ()