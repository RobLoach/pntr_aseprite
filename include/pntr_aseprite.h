/**********************************************************************************************
*
*   pntr_aseprite - pntr_aseprite sprite loader for pntr.
*
*   Copyright 2023 Rob Loach (@RobLoach)
*
*   DEPENDENCIES:
*       pntr https://github.com/robloach/pntr
*       cute_aseprite https://github.com/RandyGaul/cute_headers/blob/master/cute_aseprite.h
*
*   LICENSE: zlib/libpng
*
*   pntr_aseprite is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef PNTR_ASEPRITE_H__
#define PNTR_ASEPRITE_H__

#ifndef PNTR_ASEPRITE_API
    #define PNTR_ASEPRITE_API PNTR_API
#endif

#ifndef PNTR_ASEPRITE_CUTE_ASEPRITE_H
#define PNTR_ASEPRITE_CUTE_ASEPRITE_H "cute_aseprite.h"
#endif
#include PNTR_ASEPRITE_CUTE_ASEPRITE_H // NOLINT

/**
 * The main Aseprite object that contains the loaded Aseprite file.
 */
typedef struct pntr_aseprite {
    struct ase_t* ase; /** The cute_aseprite object */
    pntr_image* image; /** The aseprite image data */
} pntr_aseprite;

typedef struct pntr_aseprite_tag {
    char* name;         // The name of the tag.
    int currentFrame;   // The frame that the tag is currently on
    float timer;        // The countdown timer in seconds
    int direction;      // Whether we are moving forwards, or backwards through the frames
    float speed;        // The animation speed factor (1 is normal speed, 2 is double speed)
    pntr_color color;   // The color provided for the tag
    bool loop;          // Whether to continue to play the animation when the animation finishes
    bool paused;        // Set to true to not progression of the animation
    pntr_aseprite* aseprite;  // The loaded Aseprite file
    struct ase_tag_t* tag;     // The active tag to act upon
} pntr_aseprite_tag;

/*
typedef struct pntr_aseprite_slice {
    char* name;             // The name of the slice.
    pntr_rectangle bounds;  // The rectangle outer bounds for the slice.
} pntr_aseprite_slice;
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Load an .aseprite file through its memory data.
 *
 * @param fileData The loaded file data for the .aseprite file.
 * @param size The size of file in bytes.
 *
 * @return The loaded aseprite object, or NULL on failure.
 */
PNTR_ASEPRITE_API pntr_aseprite* pntr_load_aseprite(const char* fileName);

/**
 * Load an .aseprite file.
 *
 * @param fileName The path to the file to load.
 * @param size The size of the memory in bytes.
 *
 * @return The loaded aseprite object, or NULL on failure.
 */
PNTR_ASEPRITE_API pntr_aseprite* pntr_load_aseprite_from_memory(unsigned char* fileData, unsigned int size);

/**
 * Unloads the given aseprite.
 *
 * @param aseprite The aseprite object to unload.
 *
 * @see pntr_load_aseprite()
 */
PNTR_ASEPRITE_API void pntr_unload_aseprite(pntr_aseprite* aseprite);

/**
 * Get the image data from the aseprite object.
 *
 * @param aseprite The aseprite object to get the image data from.
 *
 * @return The image data from the aseprite object.
 */
PNTR_ASEPRITE_API pntr_image* pntr_aseprite_image(pntr_aseprite* aseprite);

/**
 * Get the width of the aseprite.
 *
 * @return The width of the aseprite.
 */
PNTR_ASEPRITE_API int pntr_aseprite_width(pntr_aseprite* aseprite);

/**
 * Get the height of the aseprite.
 *
 * @return The width of the aseprite.
 */
PNTR_ASEPRITE_API int pntr_aseprite_height(pntr_aseprite* aseprite);
PNTR_ASEPRITE_API void pntr_draw_aseprite(pntr_image* dst, pntr_aseprite* aseprite, int frame, int posX, int posY);
PNTR_ASEPRITE_API pntr_image pntr_aseprite_frame(pntr_aseprite* aseprite, int frame);

/**
 * Load an pntr_aseprite tag from the given name.
 *
 * @param aseprite The pntr_aseprite object to load the tag from.
 * @param name The name of the tag to be loaded.
 *
 * @return The loaded pntr_aseprite tag, or an empty tag on failure.
 *
 * @see Ispntr_aseprite_tagReady()
 * @see Updatepntr_aseprite_tag()
 */
PNTR_ASEPRITE_API pntr_aseprite_tag* pntr_load_aseprite_tag(pntr_aseprite* aseprite, const char* name);

/**
 * Load an pntr_aseprite tag from the given index.
 *
 * @param aseprite The loaded pntr_aseprite object from which to load the tag->
 * @param index The tag index within the pntr_aseprite object.
 *
 * @return The pntr_aseprite tag information, or an empty tag on failure.
 *
 * @see pntr_load_aseprite_tag()
 */
PNTR_ASEPRITE_API pntr_aseprite_tag* pntr_load_aseprite_tag_index(pntr_aseprite* aseprite, int index);
PNTR_ASEPRITE_API int pntr_aseprite_tag_count(pntr_aseprite* aseprite);

/**
 * Updates a tag to progress through its animation frame.
 *
 * @param tag The pntr_aseprite_tag passed in by reference (&tag).
 */
PNTR_ASEPRITE_API void pntr_update_aseprite_tag(pntr_aseprite_tag* tag);

/**
 * Generate an aseprite tag with sane defaults.
 *
 * @return An pntr_aseprite_tag with sane defaults.
 */
PNTR_ASEPRITE_API pntr_aseprite_tag* pntr_load_aseprite_tag_default();
PNTR_ASEPRITE_API void pntr_unload_aseprite_tag(pntr_aseprite_tag* tag);
PNTR_ASEPRITE_API void pntr_draw_aseprite_tag(pntr_image* dst, pntr_aseprite_tag* tag, int posX, int posY);

/**
 * Set which frame the pntr_aseprite tag is on.
 *
 * @param tag The pntr_aseprite tag to modify.
 * @param frameNumber Which frame to set the active tag to. If negative, will start from the end.
 */
PNTR_ASEPRITE_API void pntr_aseprite_set_tag_frame(pntr_aseprite_tag* tag, int frameNumber);
PNTR_ASEPRITE_API int pntr_aseprite_get_tag_frame(pntr_aseprite_tag* tag);

// // pntr_aseprite_slice functions
// pntr_aseprite_slice* Loadpntr_aseprite_slice(pntr_aseprite aseprite, const char* name);   // Load a slice from an pntr_aseprite based on its name.
// pntr_aseprite_slice LoadAsperiteSliceFromIndex(pntr_aseprite aseprite, int index); // Load a slice from an pntr_aseprite based on its index.
// int Getpntr_aseprite_sliceCount(pntr_aseprite aseprite);                       // Get the amount of slices that are defined in the pntr_aseprite.
// bool Ispntr_aseprite_sliceReady(pntr_aseprite_slice slice);                     // Return whether or not the given slice was found.
//pntr_aseprite_slice Genpntr_aseprite_sliceDefault();                            // Generate empty pntr_aseprite slice data.

#ifdef PNTR_ASSETSYS_API
PNTR_ASEPRITE_API pntr_aseprite* pntr_load_aseprite_from_assetsys(assetsys_t* sys, const char* path);
#endif  // PNTR_ASSETSYS_API

#ifdef __cplusplus
}
#endif

#endif  // PNTR_ASEPRITE_H__

#ifdef PNTR_ASEPRITE_IMPLEMENTATION
#ifndef PNTR_ASEPRITE_IMPLEMENTATION_ONCE
#define PNTR_ASEPRITE_IMPLEMENTATION_ONCE

#ifndef PNTR_STRCMP
#include <string.h>  // strcmp
#define PNTR_STRCMP strcmp
#endif

// TODO: Have cute_aseprite report warnings?
#define CUTE_ASEPRITE_WARNING(msg)

// For cute_aseprite asserts, use PNTR_ASSERT(). Otherwise, ignore them.
#ifdef PNTR_ASSERT
#define CUTE_ASEPRITE_ASSERT(condition) PNTR_ASSERT(condition)
#else
#define CUTE_ASEPRITE_ASSERT(condition)
#endif

#define CUTE_ASEPRITE_ALLOC(size, ctx) pntr_load_memory((int)(size))
#define CUTE_ASEPRITE_FREE(mem, ctx) pntr_unload_memory((void*)(mem))

#ifndef CUTE_ASEPRITE_MEMCPY
#define CUTE_ASEPRITE_MEMCPY PNTR_MEMCPY
#endif

#ifndef CUTE_ASEPRITE_MEMSET
#define CUTE_ASEPRITE_MEMSET PNTR_MEMSET
#endif

#define CUTE_ASEPRITE_SEEK_SET 0
#define CUTE_ASEPRITE_SEEK_END 0
#define CUTE_ASEPRITE_FILE void
#define CUTE_ASEPRITE_FOPEN(file, property) (CUTE_ASEPRITE_FILE*)file
#define CUTE_ASEPRITE_FSEEK(fp, sz, pos) (0)
#define CUTE_ASEPRITE_FREAD(data, sz, num, fp) (0)
#define CUTE_ASEPRITE_FTELL(fp) (0)
#define CUTE_ASEPRITE_FCLOSE(fp) (0)

#ifndef CUTE_ASEPRITE_IMPLEMENTATION
#define CUTE_ASEPRITE_IMPLEMENTATION
#endif
#include PNTR_ASEPRITE_CUTE_ASEPRITE_H // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

PNTR_ASEPRITE_API pntr_aseprite* pntr_load_aseprite_from_memory(unsigned char* fileData, unsigned int size) {
    pntr_aseprite* aseprite = pntr_load_memory(sizeof(pntr_aseprite));
    if (aseprite == NULL) {
        return NULL;
    }
    aseprite->ase = 0;

    ase_t* ase = cute_aseprite_load_from_memory(fileData, (int)size, 0);
    if (ase == 0 || ase->frame_count == 0 || ase->w == 0 || ase->h == 0) {
        pntr_unload_aseprite(aseprite);
        return NULL;
    }

    // Build the sprite's final image.
    pntr_image* image = pntr_gen_image_color(ase->w * ase->frame_count, ase->h, PNTR_BLANK);

    // Load all frames.
    for (int i = 0; i < ase->frame_count; i++) {
        ase_frame_t* frame = ase->frames + i;
        pntr_image* tile = pntr_image_from_pixelformat(frame->pixels, ase->w, ase->h, PNTR_PIXELFORMAT_RGBA8888);
        pntr_draw_image(image, tile, i * ase->w, 0);
        pntr_unload_image(tile);
    }

    // Apply the transparent pixel.
    int transparency  = ase->transparent_palette_entry_index;
    if (transparency >= 0 && transparency < ase->palette.entry_count) {
        ase_color_t transparentColor = ase->palette.entries[transparency].color;
        pntr_color source = pntr_new_color(
            transparentColor.r,
            transparentColor.g,
            transparentColor.b,
            transparentColor.a
        );
        pntr_image_color_replace(image, source, PNTR_BLANK);
    }
    aseprite->image = image;
    aseprite->ase = ase;

    return aseprite;
}

PNTR_ASEPRITE_API pntr_aseprite* pntr_load_aseprite(const char* fileName) {
    unsigned int bytesRead;
    unsigned char* fileData = pntr_load_file(fileName, &bytesRead);
    if (bytesRead == 0 || fileData == 0) {
        return NULL;
    }

    pntr_aseprite* ase = pntr_load_aseprite_from_memory(fileData, bytesRead);
    pntr_unload_file(fileData);
    return ase;
}

PNTR_ASEPRITE_API pntr_image* pntr_aseprite_image(pntr_aseprite* aseprite) {
    if (aseprite == NULL) {
        return NULL;
    }

    return aseprite->image;
}

PNTR_ASEPRITE_API int pntr_aseprite_width(pntr_aseprite* aseprite) {
    if (aseprite == NULL || aseprite->ase == NULL) {
        return 0;
    }

    return aseprite->ase->w;
}

PNTR_ASEPRITE_API int pntr_aseprite_height(pntr_aseprite* aseprite) {
    if (aseprite == NULL || aseprite->ase == NULL) {
        return 0;
    }

    return aseprite->ase->h;
}

PNTR_ASEPRITE_API int pntr_aseprite_tag_count(pntr_aseprite* aseprite) {
    if (aseprite == NULL || aseprite->ase == NULL) {
        return 0;
    }

    return aseprite->ase->tag_count;
}

PNTR_ASEPRITE_API void pntr_unload_aseprite(pntr_aseprite* aseprite) {
    if (aseprite == NULL) {
        return;
    }

    if (aseprite->ase != NULL) {
        cute_aseprite_free(aseprite->ase);
    }

    if (aseprite->image != NULL) {
        pntr_unload_image(aseprite->image);
    }

    pntr_unload_memory(aseprite);
}

PNTR_ASEPRITE_API void pntr_draw_aseprite(pntr_image* dst, pntr_aseprite* aseprite, int frame, int posX, int posY) {
    if (aseprite == NULL || dst == NULL) {
        return;
    }

    if (aseprite->ase == NULL || frame < 0 || frame >= aseprite->ase->frame_count) {
        return;
    }

    pntr_rectangle source = {frame * aseprite->ase->w, 0, aseprite->ase->w, aseprite->ase->h};
    pntr_draw_image_rec(dst, aseprite->image, source, posX, posY);
}

PNTR_ASEPRITE_API pntr_image pntr_aseprite_frame(pntr_aseprite* aseprite, int frame) {
    if (aseprite == NULL || aseprite->ase == NULL || frame < 0 || frame >= aseprite->ase->frame_count) {
        return PNTR_CLITERAL(pntr_image) {0};
    }

    return PNTR_CLITERAL(pntr_image) {
        .data = &PNTR_PIXEL(aseprite->image, frame * aseprite->ase->w, 0),
        .subimage = true,
        .width = aseprite->ase->w,
        .height = aseprite->ase->h,
        .pitch = aseprite->image->pitch,
        .clip = PNTR_CLITERAL(pntr_rectangle) {
            .x = 0,
            .y = 0,
            .width = aseprite->ase->w,
            .height = aseprite->ase->h
        }
    };
}

PNTR_ASEPRITE_API void pntr_aseprite_update_tag(pntr_aseprite_tag* tag, float delta_time) {
    if (tag == NULL || tag->tag == NULL || tag->aseprite == NULL) {
        return;
    }

    // Don't progress if the tag is paused.
    if (tag->paused) {
        return;
    }

    ase_t* ase = tag->aseprite->ase;
    ase_tag_t* aseTag = tag->tag;

    // Count down the timer and see if it's time to update the frame.
    tag->timer -= delta_time * tag->speed;
    if (tag->timer > 0) {
        return;
    }

    // Advance the frame and see if it's time to reset the position.
    tag->currentFrame += tag->direction;
    switch (aseTag->loop_animation_direction) {
        case ASE_ANIMATION_DIRECTION_BACKWORDS:
            if (tag->currentFrame < aseTag->from_frame) {
                if (tag->loop) {
                    tag->currentFrame = aseTag->to_frame;
                } else {
                    tag->currentFrame = aseTag->from_frame;
                    tag->paused = true;
                }
            }
        break;
        case ASE_ANIMATION_DIRECTION_PINGPONG:
            if (tag->direction > 0) {
                if (tag->currentFrame > aseTag->to_frame) {
                    tag->direction = -1;
                    if (tag->loop) {
                        tag->currentFrame = aseTag->to_frame - 1;
                    } else {
                        tag->currentFrame = aseTag->to_frame;
                        tag->paused = true;
                    }
                }
            } else {
                if (tag->currentFrame < aseTag->from_frame) {
                    tag->direction = 1;
                    if (tag->loop) {
                        tag->currentFrame = aseTag->from_frame + 1;
                    } else {
                        tag->currentFrame = aseTag->from_frame;
                        tag->paused = true;
                    }
                }
            }
        break;
        case ASE_ANIMATION_DIRECTION_FORWARDS:
        default:
            if (tag->currentFrame > aseTag->to_frame) {
                if (tag->loop) {
                    tag->currentFrame = aseTag->from_frame;
                } else {
                    tag->currentFrame = aseTag->to_frame;
                    tag->paused = true;
                }
            }
        break;
    }

    // Reset the timer, considering the offset of the animation.
    tag->timer += (float)(ase->frames[tag->currentFrame].duration_milliseconds) / 1000.0f;
}

PNTR_ASEPRITE_API void pntr_aseprite_set_tag_frame(pntr_aseprite_tag* tag, int frameNumber) {
    // TODO: Need to attribute frame number for ASE_ANIMATION_DIRECTION_BACKWORDS?
    if (frameNumber >= 0) {
        tag->currentFrame = tag->tag->from_frame + frameNumber;
    }
    else {
        tag->currentFrame = tag->tag->to_frame + frameNumber;
    }

    // Bounds
    if (tag->currentFrame < tag->tag->from_frame) {
        tag->currentFrame = tag->tag->from_frame;
    }
    else if (tag->currentFrame > tag->tag->to_frame) {
        tag->currentFrame = tag->tag->to_frame;
    }
}

PNTR_ASEPRITE_API int pntr_aseprite_get_tag_frame(pntr_aseprite_tag* tag) {
    if (tag == NULL) {
        return -1;
    }

    // TODO: Need to attribute frame number for ASE_ANIMATION_DIRECTION_BACKWORDS?
    return tag->currentFrame - tag->tag->from_frame;
}

PNTR_ASEPRITE_API void pntr_draw_aseprite_tag(pntr_image* dst, pntr_aseprite_tag* tag, int posX, int posY) {
    if (tag == NULL) {
        return;
    }
    pntr_draw_aseprite(dst, tag->aseprite, tag->currentFrame, posX, posY);
}

PNTR_ASEPRITE_API pntr_aseprite_tag* pntr_load_aseprite_tag_default() {
    pntr_aseprite_tag* tag = pntr_load_memory(sizeof(pntr_aseprite_tag));
    if (tag == NULL) {
        return NULL;
    }

    tag->aseprite = NULL;
    tag->currentFrame = 0;
    tag->tag = 0;
    tag->timer = 0;
    tag->direction = 0;
    tag->speed = 1.0f;
    tag->color = PNTR_BLACK;
    tag->loop = true;
    tag->paused = false;
    tag->name = 0;
    return tag;
}

PNTR_ASEPRITE_API pntr_aseprite_tag* pntr_load_aseprite_tag_index(pntr_aseprite* aseprite, int index) {
    if (aseprite == NULL) {
        return NULL;
    }

    // Ensure the pntr_aseprite exists.
    ase_t* ase = aseprite->ase;
    if (ase == NULL) {
        return NULL;
    }

    // Ensure the tag exists
    if (index < 0 || index >= ase->tag_count) {
        return NULL;
    }

    pntr_aseprite_tag* tag = pntr_load_aseprite_tag_default();
    if (tag == NULL) {
        return NULL;
    }

    // Base tag information
    tag->aseprite = aseprite;
    tag->tag = &ase->tags[index];

    // Set up the frame range
    tag->direction = 1;
    tag->currentFrame = tag->tag->from_frame;
    if (tag->tag->loop_animation_direction == ASE_ANIMATION_DIRECTION_BACKWORDS) {
        tag->currentFrame = tag->tag->to_frame;
        tag->direction = -1;
    }

    // Pause the animation if it's a one-frame tag
    if (tag->tag->from_frame == tag->tag->to_frame) {
        tag->paused = true;
    }

    // Timer in seconds
    tag->timer = (float)(ase->frames[tag->currentFrame].duration_milliseconds) / 1000.0f;

    // Color
    pntr_color_set_r(&tag->color, (unsigned char)tag->tag->r);
    pntr_color_set_g(&tag->color, (unsigned char)tag->tag->g);
    pntr_color_set_b(&tag->color, (unsigned char)tag->tag->b);

    // Name
    tag->name = (char*)tag->tag->name;

    return tag;
}

PNTR_ASEPRITE_API pntr_aseprite_tag* pntr_load_aseprite_tag(pntr_aseprite* aseprite, const char* name) {
    if (aseprite == NULL) {
        return NULL;
    }

    // Ensure the pntr_aseprite exists.
    ase_t* ase = aseprite->ase;
    if (ase == NULL) {
        return NULL;
    }

    // Loop through all tags to find the correct name.
    for (int i = 0; i < ase->tag_count; i++) {
        if (PNTR_STRCMP(name, ase->tags[i].name) == 0) {
            return pntr_load_aseprite_tag_index(aseprite, i);
        }
    }

    return NULL;
}

PNTR_ASEPRITE_API void pntr_unload_aseprite_tag(pntr_aseprite_tag* tag) {
    pntr_unload_memory(tag);
}

/**
 * Load a slice from an pntr_aseprite based on its name.
 *
 * @param name The name of the slice to find.
 *
 * @return The loaded slice, or an empty one if not found.
 */
// pntr_aseprite_slice Loadpntr_aseprite_slice(pntr_aseprite aseprite, const char* name) {
//     if (aseprite.ase == NULL) {
//         TraceLog(LOG_WARNING, "ASEPRITE: Cannot load slice on empty aseprite");
//         return Genpntr_aseprite_sliceDefault();
//     }
//     for (int i = 0; i < aseprite.ase->slice_count; i++) {
//         ase_slice_t* slice = &aseprite.ase->slices[i];
//         if (TextIsEqual(name, slice->name)) {
//             return LoadAsperiteSliceFromIndex(aseprite, i);
//         }
//     }

//     return Genpntr_aseprite_sliceDefault();
// }

/**
 * Load a slice from an pntr_aseprite based on its index.
 *
 * @param index The index of the slice to load.
 *
 * @return The loaded slice, or an empty one if not found.
 */
// pntr_aseprite_slice LoadAsperiteSliceFromIndex(pntr_aseprite aseprite, int index) {
//     if (aseprite.ase == NULL) {
//         TraceLog(LOG_WARNING, "ASEPRITE: Cannot load slice index from empty aseprite");
//         return Genpntr_aseprite_sliceDefault();
//     }
//     if (index < aseprite.ase->slice_count) {
//         pntr_aseprite_slice output;
//         ase_slice_t* slice = &aseprite.ase->slices[index];
//         output.bounds.x = (float)slice->origin_x;
//         output.bounds.y = (float)slice->origin_y;
//         output.bounds.width = (float)slice->w;
//         output.bounds.height = (float)slice->h;
//         output.name = (char*)slice->name;
//         return output;
//     }

//     return Genpntr_aseprite_sliceDefault();
// }

/**
 * Generate empty pntr_aseprite slice data.
 */
// pntr_aseprite_slice Genpntr_aseprite_sliceDefault() {
//     pntr_aseprite_slice slice;
//     slice.name = "";
//     slice.bounds = (Rectangle){0, 0, 0, 0};
//     return slice;
// }

/**
 * Get the amount of slices that are defined in the pntr_aseprite.
 *
 * @return The amount of slices.
 */
// int Getpntr_aseprite_sliceCount(pntr_aseprite aseprite) {
//     return aseprite.ase->slice_count;
// }

/**
 * Return whether or not the given slice was found.
 */
// bool Ispntr_aseprite_sliceReady(pntr_aseprite_slice slice) {
//     return TextLength(slice.name) != 0;
// }

#ifdef PNTR_ASSETSYS_API
PNTR_ASEPRITE_API pntr_aseprite* pntr_load_aseprite_from_assetsys(assetsys_t* sys, const char* path) {
    unsigned int size;
    unsigned char* data = pntr_load_file_from_assetsys(sys, path, &size);
    if (data == NULL) {
        return NULL;
    }

    return pntr_load_aseprite_from_memory(data, size);
}
#endif  // PNTR_ASSETSYS_API

#ifdef __cplusplus
}
#endif

#endif  // PNTR_ASEPRITE_IMPLEMENTATION_ONCE
#endif  // PNTR_ASEPRITE_IMPLEMENTATION
