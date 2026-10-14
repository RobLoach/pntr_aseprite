#include <stdio.h>

#define PNTR_IMPLEMENTATION
#include "pntr.h"
#include "pntr_assert.h"

// Count the parser's warnings, so the test can check that they are surfaced.
static int warningCount = 0;
void test_warning(const char* message) {
    (void)message;
    warningCount++;
}
#define PNTR_ASEPRITE_WARNING(message) test_warning(message)

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

/**
 * Stepping a tag with more time than the frame's duration must advance the frame.
 */
void test_update_tag(pntr_aseprite* aseprite) {
    pntr_aseprite_tag* tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    PNTR_ASSERT(tag);
    PNTR_ASSERT_EQUALS(tag->currentFrame, 1);
    PNTR_ASSERT_EQUALS(pntr_aseprite_get_tag_frame(tag), 0);

    // Every frame in george.aseprite lasts 200ms, so a larger step advances the frame.
    pntr_aseprite_update_tag(tag, 0.25f);
    PNTR_ASSERT_EQUALS(tag->currentFrame, 2);
    PNTR_ASSERT_EQUALS(pntr_aseprite_get_tag_frame(tag), 1);

    // A step that doesn't run the timer out leaves the frame where it was.
    pntr_aseprite_update_tag(tag, 0.01f);
    PNTR_ASSERT_EQUALS(tag->currentFrame, 2);

    // Updating nothing must not crash.
    pntr_aseprite_update_tag(NULL, 0.25f);

    pntr_unload_aseprite_tag(tag);
}

/**
 * The frame accessors must tolerate a tag that has no ase_tag_t behind it, which is
 * exactly what pntr_load_aseprite_tag_default() hands out.
 */
void test_tag_frame_without_tag() {
    pntr_aseprite_tag* tag = pntr_load_aseprite_tag_default();
    PNTR_ASSERT(tag);
    PNTR_ASSERT(tag->tag == NULL);

    pntr_aseprite_set_tag_frame(tag, 3);
    PNTR_ASSERT_EQUALS(pntr_aseprite_get_tag_frame(tag), -1);

    pntr_unload_aseprite_tag(tag);

    // And no tag at all.
    pntr_aseprite_set_tag_frame(NULL, 3);
    PNTR_ASSERT_EQUALS(pntr_aseprite_get_tag_frame(NULL), -1);
}

/**
 * An RGBA sprite that happens to carry a palette must keep every opaque pixel, even
 * one whose color is identical to the transparent palette entry.
 *
 * rgba_palette.aseprite is a hand-built 2x1 single-frame RGBA file holding one old
 * palette chunk (0x0004) whose only entry is an opaque rgb(10, 20, 30), a
 * transparent_palette_entry_index of 0, and one raw cel of two opaque pixels:
 * rgba(10, 20, 30, 255), which matches that palette entry, and rgba(200, 100, 50, 255),
 * which does not. The transparent palette entry is only meaningful for indexed
 * sprites, so neither pixel may be touched here.
 */
void test_rgba_mode_keeps_palette_colored_pixels() {
    pntr_aseprite* aseprite = pntr_load_aseprite("resources/rgba_palette.aseprite");
    PNTR_ASSERT(aseprite);
    PNTR_ASSERT_EQUALS(pntr_aseprite_width(aseprite), 2);
    PNTR_ASSERT_EQUALS(pntr_aseprite_height(aseprite), 1);

    pntr_image* image = pntr_aseprite_image(aseprite);
    PNTR_ASSERT(image);

    PNTR_ASSERT_COLOR_EQUALS(pntr_image_get_color(image, 0, 0), pntr_new_color(10, 20, 30, 255));
    PNTR_ASSERT_COLOR_EQUALS(pntr_image_get_color(image, 1, 0), pntr_new_color(200, 100, 50, 255));

    pntr_unload_aseprite(aseprite);
}

/**
 * Only the Normal blend mode is composited, so the parser's complaint about any other
 * one must reach the configured PNTR_ASEPRITE_WARNING() handler rather than be dropped.
 *
 * multiply.aseprite is rgba_palette.aseprite with its layer's blend mode set to
 * Multiply (3) instead of Normal (0).
 */
void test_blend_mode_warning() {
    // Everything loaded so far uses Normal, so nothing should have warned yet.
    PNTR_ASSERT_EQUALS(warningCount, 0);

    pntr_aseprite* aseprite = pntr_load_aseprite("resources/multiply.aseprite");
    PNTR_ASSERT(aseprite);
    PNTR_ASSERT_EQUALS(warningCount, 1);

    pntr_unload_aseprite(aseprite);
    warningCount = 0;
}

int main() {
    test_load_failures();
    test_tag_frame_without_tag();
    test_rgba_mode_keeps_palette_colored_pixels();
    test_blend_mode_warning();

    pntr_aseprite* aseprite = pntr_load_aseprite("resources/george.aseprite");
    PNTR_ASSERT(aseprite);

    test_update_tag(aseprite);

    pntr_image* output = pntr_gen_image_color(100, 100, PNTR_WHITE);
    PNTR_ASSERT(output);

    pntr_draw_aseprite(output, aseprite, 6, 10, 10);
    pntr_draw_aseprite(output, aseprite, 10, 50, 10);

    pntr_save_image(output, "pntr_aseprite_test.png");

    pntr_unload_image(output);
    pntr_unload_aseprite(aseprite);

    return 0;
}
