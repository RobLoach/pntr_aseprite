cmake_minimum_required(VERSION 3.11)
project(pntr_aseprite
    DESCRIPTION "pntr_aseprite: Sprites from aseprite with pntr"
    HOMEPAGE_URL "https://github.com/robloach/pntr_aseprite"
    VERSION 0.1.0
    LANGUAGES C
)

# pntr_aseprite
add_library(pntr_aseprite INTERFACE)
target_include_directories(pntr_aseprite INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Options
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    set(PNTR_ASEPRITE_IS_MAIN TRUE)
else()
    set(PNTR_ASEPRITE_IS_MAIN FALSE)
endif()

option(PNTR_ASEPRITE_BUILD_EXAMPLES "pntr_aseprite: Examples" ${PNTR_ASEPRITE_IS_MAIN})
option(PNTR_ASEPRITE_BUILD_EXAMPLES_RAYLIB "pntr_aseprite: raylib Examples" ${PNTR_ASEPRITE_IS_MAIN})
option(PNTR_ASEPRITE_BUILD_TESTS "pntr_aseprite: Test" ${PNTR_ASEPRITE_IS_MAIN})

if (PNTR_ASEPRITE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Testing
if (PNTR_ASEPRITE_BUILD_TESTS)
    include(CTest)
    enable_testing()
    if (BUILD_TESTING)
        set(CTEST_CUSTOM_TESTS_IGNORE
            pkg-config--static
        )
        # Always print verbose output when tests fail if run using `make test`.
        list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
        add_subdirectory(test)
    endif()
endif()
