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

add_subdirectory(vendor)
add_subdirectory(src)