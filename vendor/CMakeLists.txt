add_subdirectory(cppargparse)

set(CPPARGPARSE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/cppargparse/argparse PARENT_SCOPE)

if (PROJECTEMERALD_ENABLE_TESTS)
    add_subdirectory(googletest)
endif ()