# pntr_aseprite

Load [Aseprite](https://www.aseprite.org/) animated sprites for use in [pntr](https://github.com/robloach/pntr).

## Usage

``` c
#define PNTR_IMPLEMENTATION
#include "pntr.h"

#define PNTR_ASEPRITE_IMPLEMENTATION
#include "pntr_aseprite.h"

int main() {
    pntr_aseprite* aseprite = pntr_load_aseprite("sprite.aseprite");
    pntr_image* output = pntr_gen_image_color(100, 100, PNTR_WHITE);

    int posX = 10;
    int posY = 10;
    int frameNumber = 0;
    pntr_draw_aseprite(output, aseprite, frameNumber, posX, posY);

    pntr_save_image(output, "output.png");

    pntr_unload_image(output);
    pntr_unload_aseprite(aseprite);
    return 0;
}
```

### Animation

Aseprite tags carry their own frame range and timings. Load one, step it with how
much time has passed, and draw whichever frame it lands on:

``` c
pntr_aseprite* aseprite = pntr_load_aseprite("sprite.aseprite");
pntr_aseprite_tag* tag = pntr_load_aseprite_tag(aseprite, "Walk-Down");

// Each time around your game loop...
pntr_aseprite_update_tag(tag, deltaTime);
pntr_draw_aseprite_tag(screen, tag, posX, posY);

pntr_unload_aseprite_tag(tag);
pntr_unload_aseprite(aseprite);
```

### API

``` c
// pntr_aseprite functions
pntr_aseprite* pntr_load_aseprite(const char* fileName);
pntr_aseprite* pntr_load_aseprite_from_memory(unsigned char* fileData, unsigned int size);
void pntr_unload_aseprite(pntr_aseprite* aseprite);
pntr_image* pntr_aseprite_image(pntr_aseprite* aseprite);
int pntr_aseprite_width(pntr_aseprite* aseprite);
int pntr_aseprite_height(pntr_aseprite* aseprite);
void pntr_draw_aseprite(pntr_image* dst, pntr_aseprite* aseprite, int frame, int posX, int posY);
pntr_image pntr_aseprite_frame(pntr_aseprite* aseprite, int frame);

// // pntr_aseprite_tag functions
pntr_aseprite_tag* pntr_load_aseprite_tag(pntr_aseprite* aseprite, const char* name);
pntr_aseprite_tag* pntr_load_aseprite_tag_index(pntr_aseprite* aseprite, int index);
int pntr_aseprite_tag_count(pntr_aseprite* aseprite);
void pntr_aseprite_update_tag(pntr_aseprite_tag* tag, float delta_time);
pntr_aseprite_tag* pntr_load_aseprite_tag_default();
void pntr_unload_aseprite_tag(pntr_aseprite_tag* tag);
void pntr_draw_aseprite_tag(pntr_image* dst, pntr_aseprite_tag* tag, int posX, int posY);
void pntr_aseprite_set_tag_frame(pntr_aseprite_tag* tag, int frameNumber);
int pntr_aseprite_get_tag_frame(pntr_aseprite_tag* tag);
```

### Limitations

Layers are composited with the **Normal** blend mode only. Layer and cel opacity are
applied, but a layer set to Multiply, Screen, Overlay, Add or any other mode is
composited as though it were Normal, so it will not match what Aseprite shows.

Loading reports this, and anything else it finds, through `PNTR_ASEPRITE_WARNING()`.
It does nothing by default, so define it before including the header to see them:

``` c
void MyWarning(const char* message) {
    printf("pntr_aseprite: %s\n", message);
}
#define PNTR_ASEPRITE_WARNING(message) MyWarning(message)

#define PNTR_ASEPRITE_IMPLEMENTATION
#include "pntr_aseprite.h"
```

## Credits

- [George Sprite](https://opengameart.org/content/alternate-lpc-character-sprites-george) by [Sheep](https://opengameart.org/users/sheep)
- [cute_aseprite.h](https://github.com/RandyGaul/cute_headers/blob/master/cute_aseprite.h) by [RandyGaul](https://github.com/RandyGaul)

## License

Unless stated otherwise, all works are:

- Copyright (c) 2023 [Rob Loach](https://robloach.net)

... and licensed under:

- [zlib License](LICENSE)
