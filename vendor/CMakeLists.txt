add_subdirectory(sage)

if (PROJECTEMERALD_ENABLE_TESTS)
    add_subdirectory(googletest)
endif ()