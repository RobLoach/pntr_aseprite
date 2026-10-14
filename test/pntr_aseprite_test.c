#include <stdio.h>

#define PNTR_IMPLEMENTATION
#include "pntr.h"
#include "pntr_assert.h"

#define PNTR_ASEPRITE_IMPLEMENTATION
#include "pntr_aseprite.h"

/**
 * Loading a malformed .aseprite file must fail cleanly, without leaking the parsed
 * file or freeing an uninitialized image pointer.
 */
void test_load_failures() {
    // A missing file.
    PNTR_ASSERT(pntr_load_aseprite("resources/does_not_exist.aseprite") == NULL);

    // An empty buffer.
    PNTR_ASSERT(pntr_load_aseprite_from_memory(NULL, 0) == NULL);
    PNTR_ASSERT(pntr_load_aseprite_from_memory(NULL, 128) == NULL);

    // A header-only file that parses, but carries no frames, so it is rejected after
    // cute_aseprite has already allocated for it.
    PNTR_ASSERT(pntr_load_aseprite("resources/zero_frames.aseprite") == NULL);
}

int main() {
    test_load_failures();

    pntr_aseprite* aseprite = pntr_load_aseprite("resources/george.aseprite");
    PNTR_ASSERT(aseprite);

    pntr_image* output = pntr_gen_image_color(100, 100, PNTR_WHITE);
    PNTR_ASSERT(output);

    pntr_draw_aseprite(output, aseprite, 6, 10, 10);
    pntr_draw_aseprite(output, aseprite, 10, 50, 10);

    pntr_save_image(output, "pntr_aseprite_test.png");

    pntr_unload_image(output);
    pntr_unload_aseprite(aseprite);

    return 0;
}
