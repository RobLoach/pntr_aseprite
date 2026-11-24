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
    // cute_aseprite has already allocated for it. See tools/generate_fixtures.py.
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
 * A single step spanning several frame durations must advance every one of them.
 */
void test_update_tag_catches_up(pntr_aseprite* aseprite) {
    pntr_aseprite_tag* tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    PNTR_ASSERT(tag);
    PNTR_ASSERT_EQUALS(tag->currentFrame, 1);
    PNTR_ASSERT(tag->tag->to_frame >= 3);

    // Two whole frame durations plus a bit of the third.
    float first = (float)aseprite->ase->frames[1].duration_milliseconds / 1000.0f;
    float second = (float)aseprite->ase->frames[2].duration_milliseconds / 1000.0f;
    float third = (float)aseprite->ase->frames[3].duration_milliseconds / 1000.0f;
    pntr_aseprite_update_tag(tag, first + second + third / 4.0f);
    PNTR_ASSERT_EQUALS(tag->currentFrame, 3);
    PNTR_ASSERT_EQUALS(pntr_aseprite_get_tag_frame(tag), 2);

    pntr_unload_aseprite_tag(tag);
}

/**
 * The frame count and duration accessors must report the parsed values, and 0 for
 * anything out of range.
 */
void test_frame_count_and_duration(pntr_aseprite* aseprite) {
    int count = pntr_aseprite_frame_count(aseprite);
    PNTR_ASSERT(count > 0);
    PNTR_ASSERT_EQUALS(count, aseprite->ase->frame_count);
    PNTR_ASSERT_EQUALS(pntr_aseprite_frame_count(NULL), 0);

    float expected = (float)aseprite->ase->frames[0].duration_milliseconds / 1000.0f;
    PNTR_ASSERT(pntr_aseprite_frame_duration(aseprite, 0) == expected);
    PNTR_ASSERT(pntr_aseprite_frame_duration(aseprite, 0) > 0.0f);

    PNTR_ASSERT(pntr_aseprite_frame_duration(NULL, 0) == 0.0f);
    PNTR_ASSERT(pntr_aseprite_frame_duration(aseprite, -1) == 0.0f);
    PNTR_ASSERT(pntr_aseprite_frame_duration(aseprite, count) == 0.0f);
}

/**
 * Steps the tag through each of the given frames, one frame duration at a time.
 */
void assert_tag_frames(pntr_aseprite_tag* tag, const int* frames, int count) {
    for (int i = 0; i < count; i++) {
        PNTR_ASSERT_EQUALS(tag->currentFrame, frames[i]);
        pntr_aseprite_update_tag(tag, (float)tag->aseprite->ase->frames[tag->currentFrame].duration_milliseconds / 1000.0f);
    }
}

/**
 * The tag's repeat count decides whether it loops, and how many times it plays.
 */
void test_update_tag_repeat(pntr_aseprite* aseprite) {
    // Walk-Down is a ping-pong tag over frames 1 to 3, set to loop forever.
    pntr_aseprite_tag* tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    PNTR_ASSERT(tag);
    PNTR_ASSERT_EQUALS(tag->tag->loop_animation_direction, ASE_ANIMATION_DIRECTION_PINGPONG);
    PNTR_ASSERT_EQUALS(tag->tag->repeat, 0);
    PNTR_ASSERT(tag->loop);

    // Looping ping-pong keeps bouncing between the ends.
    const int looping[] = {1, 2, 3, 2, 1, 2, 3, 2, 1};
    assert_tag_frames(tag, looping, 9);
    PNTR_ASSERT(!tag->paused);
    pntr_unload_aseprite_tag(tag);

    // Without looping, ping-pong plays a single pass, and stops at the far end.
    tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    tag->loop = false;
    const int pingpong[] = {1, 2, 3, 3, 3};
    assert_tag_frames(tag, pingpong, 5);
    PNTR_ASSERT(tag->paused);
    pntr_unload_aseprite_tag(tag);

    // Ping-pong reverse starts at the end, and its single pass goes back to the start.
    aseprite->ase->tags[1].loop_animation_direction = (ase_animation_direction_t)3;
    tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    tag->loop = false;
    const int pingpongReverse[] = {3, 2, 1, 1};
    assert_tag_frames(tag, pingpongReverse, 4);
    PNTR_ASSERT(tag->paused);
    pntr_unload_aseprite_tag(tag);

    // Forward without looping stops on the last frame.
    aseprite->ase->tags[1].loop_animation_direction = ASE_ANIMATION_DIRECTION_FORWARDS;
    tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    tag->loop = false;
    const int forward[] = {1, 2, 3, 3, 3};
    assert_tag_frames(tag, forward, 5);
    PNTR_ASSERT(tag->paused);
    pntr_unload_aseprite_tag(tag);

    // A repeat count of 2 turns looping off, and plays the tag twice.
    aseprite->ase->tags[1].repeat = 2;
    tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    PNTR_ASSERT(!tag->loop);
    PNTR_ASSERT_EQUALS(tag->repeat, 2);
    const int repeated[] = {1, 2, 3, 1, 2, 3, 3};
    assert_tag_frames(tag, repeated, 7);
    PNTR_ASSERT(tag->paused);

    // Turning looping back on overrides the repeat count.
    tag->loop = true;
    tag->paused = false;
    const int resumed[] = {3, 1, 2, 3, 1};
    assert_tag_frames(tag, resumed, 5);
    PNTR_ASSERT(!tag->paused);
    pntr_unload_aseprite_tag(tag);

    // Each ping-pong direction is one repeat, so twice goes forward then back.
    aseprite->ase->tags[1].loop_animation_direction = ASE_ANIMATION_DIRECTION_PINGPONG;
    tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    const int pingpongTwice[] = {1, 2, 3, 2, 1, 1};
    assert_tag_frames(tag, pingpongTwice, 6);
    PNTR_ASSERT(tag->paused);
    pntr_unload_aseprite_tag(tag);

    // And three times goes forward, back, and forward again.
    aseprite->ase->tags[1].repeat = 3;
    tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    const int pingpongThrice[] = {1, 2, 3, 2, 1, 2, 3, 3};
    assert_tag_frames(tag, pingpongThrice, 8);
    PNTR_ASSERT(tag->paused);
    pntr_unload_aseprite_tag(tag);

    // Restore the tag for the other tests.
    aseprite->ase->tags[1].repeat = 0;
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
 * Setting a tag's frame counts from the start of the tag, or from its end when
 * negative, clamps to the tag, and restarts the timer for the new frame.
 */
void test_set_tag_frame(pntr_aseprite* aseprite) {
    // Walk-Down covers frames 1 to 3.
    pntr_aseprite_tag* tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");
    PNTR_ASSERT(tag);
    int from = tag->tag->from_frame;
    int to = tag->tag->to_frame;
    PNTR_ASSERT_EQUALS(from, 1);
    PNTR_ASSERT_EQUALS(to, 3);

    // Non-negative values round-trip.
    for (int i = 0; i <= to - from; i++) {
        pntr_aseprite_set_tag_frame(tag, i);
        PNTR_ASSERT_EQUALS(tag->currentFrame, from + i);
        PNTR_ASSERT_EQUALS(pntr_aseprite_get_tag_frame(tag), i);
    }

    // Negative values count from the end.
    pntr_aseprite_set_tag_frame(tag, -1);
    PNTR_ASSERT_EQUALS(tag->currentFrame, to);
    pntr_aseprite_set_tag_frame(tag, -2);
    PNTR_ASSERT_EQUALS(tag->currentFrame, to - 1);

    // Out of range values clamp.
    pntr_aseprite_set_tag_frame(tag, 100);
    PNTR_ASSERT_EQUALS(tag->currentFrame, to);
    pntr_aseprite_set_tag_frame(tag, -100);
    PNTR_ASSERT_EQUALS(tag->currentFrame, from);

    // The timer restarts with the new frame's duration.
    pntr_aseprite_update_tag(tag, 0.01f);
    pntr_aseprite_set_tag_frame(tag, 1);
    PNTR_ASSERT(tag->timer == (float)aseprite->ase->frames[from + 1].duration_milliseconds / 1000.0f);

    pntr_unload_aseprite_tag(tag);
}

/**
 * An RGBA sprite that happens to carry a palette must keep every opaque pixel, even
 * one whose color is identical to the transparent palette entry.
 *
 * rgba_palette.aseprite is built by tools/generate_fixtures.py.
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
 * multiply.aseprite is built by tools/generate_fixtures.py.
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
    test_update_tag_catches_up(aseprite);
    test_frame_count_and_duration(aseprite);
    test_update_tag_repeat(aseprite);
    test_set_tag_frame(aseprite);

    pntr_image* output = pntr_gen_image_color(100, 100, PNTR_WHITE);
    PNTR_ASSERT(output);

    pntr_draw_aseprite(output, aseprite, 6, 10, 10);
    pntr_draw_aseprite(output, aseprite, 10, 50, 10);

    pntr_save_image(output, "pntr_aseprite_test.png");

    pntr_unload_image(output);
    pntr_unload_aseprite(aseprite);

    return 0;
}
