# Add the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

find_package(pntr)

# Resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

# pntr_aseprite_test
add_executable(pntr_aseprite_test pntr_aseprite_test.c)
target_link_libraries(pntr_aseprite_test PUBLIC
    pntr
    pntr_aseprite
)
set_property(TARGET pntr_aseprite_test PROPERTY C_STANDARD 99)

# Warnings are enabled here rather than on the pntr_aseprite interface target, so that
# consumers don't inherit them.
if (MSVC)
    target_compile_options(pntr_aseprite_test PRIVATE /W4)
else()
    target_compile_options(pntr_aseprite_test PRIVATE -Wall -Wextra)
endif()

# Set up the test
add_test(NAME pntr_aseprite_test COMMAND pntr_aseprite_test)
