#!/usr/bin/env python3
"""Generate the synthetic .aseprite test fixtures in test/resources/.

Each fixture is described declaratively in FIXTURES below and serialized here
following the Aseprite file format spec:
https://github.com/aseprite/aseprite/blob/main/docs/ase-file-specs.md

    tools/generate_fixtures.py           # (re)write every fixture
    tools/generate_fixtures.py --check   # exit non-zero if a committed one differs

To add a fixture, add an entry to FIXTURES. george.aseprite is real artwork and
is not generated.
"""

import argparse
import os
import struct
import sys

RESOURCES = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "test", "resources")

HEADER_MAGIC = 0xA5E0
FRAME_MAGIC = 0xF1FA
CHUNK_OLD_PALETTE = 0x0004
CHUNK_LAYER = 0x2004
CHUNK_CEL = 0x2005

BLEND_NORMAL = 0
BLEND_MULTIPLY = 3

# A 2x1 single-frame RGBA sprite holding one old palette chunk (0x0004) whose only
# entry is an opaque rgb(10, 20, 30), a transparent_palette_entry_index of 0, and one
# raw cel of two opaque pixels: rgba(10, 20, 30, 255), which matches that palette
# entry, and rgba(200, 100, 50, 255), which does not. The transparent palette entry is
# only meaningful for indexed sprites, so a loader must keep both pixels opaque.
RGBA_PALETTE = {
    "width": 2,
    "height": 1,
    "transparent_index": 0,
    "palette": [(10, 20, 30)],
    "frames": [
        {
            "duration": 100,
            "layers": [{"name": "Layer 1", "blend_mode": BLEND_NORMAL}],
            "cels": [
                {"layer": 0, "width": 2, "height": 1,
                 "pixels": [(10, 20, 30, 255), (200, 100, 50, 255)]},
            ],
        },
    ],
}


def with_blend_mode(sprite, blend_mode):
    """Copy a sprite description with every layer set to the given blend mode."""
    frames = []
    for frame in sprite["frames"]:
        layers = [dict(layer, blend_mode=blend_mode) for layer in frame["layers"]]
        frames.append(dict(frame, layers=layers))
    return dict(sprite, frames=frames)


FIXTURES = {
    # A 16x16 RGBA header with no frames at all: it parses, but has nothing to draw,
    # so pntr_load_aseprite() must reject it after cute_aseprite has allocated for it.
    "zero_frames.aseprite": {"width": 16, "height": 16, "frames": []},

    "rgba_palette.aseprite": RGBA_PALETTE,

    # rgba_palette.aseprite with its layer's blend mode set to Multiply (3) instead of
    # Normal (0), so loading it must raise one PNTR_ASEPRITE_WARNING().
    "multiply.aseprite": with_blend_mode(RGBA_PALETTE, BLEND_MULTIPLY),
}


def chunk(chunk_type, data):
    return struct.pack("<IH", 6 + len(data), chunk_type) + data


def string(text):
    encoded = text.encode("utf-8")
    return struct.pack("<H", len(encoded)) + encoded


def old_palette_chunk(colors):
    # One packet: skip 0 entries, then the colors (a count of 0 means 256).
    data = struct.pack("<HBB", 1, 0, len(colors) % 256)
    for color in colors:
        data += bytes(color)
    return chunk(CHUNK_OLD_PALETTE, data)


def layer_chunk(layer):
    data = struct.pack(
        "<HHHHHHB3x",
        layer.get("flags", 1),  # Visible.
        0,  # Normal image layer.
        0,  # Child level.
        0, 0,  # Default width/height (ignored).
        layer.get("blend_mode", BLEND_NORMAL),
        layer.get("opacity", 255),
    )
    return chunk(CHUNK_LAYER, data + string(layer["name"]))


def cel_chunk(cel):
    data = struct.pack(
        "<HhhBHh5xHH",
        cel["layer"],
        cel.get("x", 0), cel.get("y", 0),
        cel.get("opacity", 255),
        0,  # Raw image data.
        0,  # z-index.
        cel["width"], cel["height"],
    )
    for pixel in cel["pixels"]:
        data += bytes(pixel)
    return chunk(CHUNK_CEL, data)


def frame(sprite, frame_index, description):
    chunks = []
    # The palette and layers live in the first frame.
    if frame_index == 0:
        if sprite.get("palette"):
            chunks.append(old_palette_chunk(sprite["palette"]))
        chunks.extend(layer_chunk(layer) for layer in description.get("layers", []))
    chunks.extend(cel_chunk(cel) for cel in description.get("cels", []))
    body = b"".join(chunks)
    header = struct.pack(
        "<IHHH2xI",
        16 + len(body),
        FRAME_MAGIC,
        min(len(chunks), 0xFFFF),
        description.get("duration", 100),
        len(chunks),
    )
    return header + body


def build(sprite):
    frames = b"".join(frame(sprite, i, f) for i, f in enumerate(sprite["frames"]))
    header = struct.pack(
        "<IHHHHHIH8xB3xHBBhhHH84x",
        128 + len(frames),
        HEADER_MAGIC,
        len(sprite["frames"]),
        sprite["width"],
        sprite["height"],
        32,  # RGBA color depth.
        1,  # Flags: layer opacity has a valid value.
        100,  # Deprecated speed.
        sprite.get("transparent_index", 0),
        len(sprite.get("palette", [])),
        0, 0,  # Pixel ratio.
        0, 0, 0, 0,  # Grid.
    )
    assert len(header) == 128
    return header + frames


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "--check",
        action="store_true",
        help="compare against the committed fixtures instead of writing them",
    )
    args = parser.parse_args()

    stale = 0
    for name, sprite in FIXTURES.items():
        path = os.path.normpath(os.path.join(RESOURCES, name))
        generated = build(sprite)
        if args.check:
            try:
                with open(path, "rb") as handle:
                    existing = handle.read()
            except FileNotFoundError:
                existing = None
            if existing == generated:
                print("%s is up to date" % name, file=sys.stderr)
            else:
                print("%s differs from the generator" % name, file=sys.stderr)
                stale += 1
        else:
            with open(path, "wb") as handle:
                handle.write(generated)
            print("wrote %s (%d bytes)" % (name, len(generated)), file=sys.stderr)

    if stale:
        print("run tools/generate_fixtures.py to regenerate", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
