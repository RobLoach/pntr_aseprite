#include <stdio.h>

#define PNTR_DISABLE_MATH
#define PNTR_IMPLEMENTATION
//#define PNTR_ENABLE_DEFAULT_FONT
#include "pntr.h"
#include "pntr_assert.h"

#define PNTR_ASEPRITE_IMPLEMENTATION
#include "pntr_aseprite.h"

int main() {
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
