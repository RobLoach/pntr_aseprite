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
    pntr_draw_aseprite(output, sprite, frameNumber, 10, 10);

    pntr_save_image(output, "output.png");

    pntr_unload_aseprite(sprite);
    return 0;
}
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

// // pntr_aseprite_tag functions
pntr_aseprite_tag* pntr_load_aseprite_tag(pntr_aseprite* aseprite, const char* name);
pntr_aseprite_tag* pntr_load_aseprite_tag_index(pntr_aseprite* aseprite, int index);
int pntr_aseprite_tag_count(pntr_aseprite* aseprite);
void pntr_update_aseprite_tag(pntr_aseprite_tag* tag);
pntr_aseprite_tag* pntr_load_aseprite_tag_default();
void pntr_unload_aseprite_tag(pntr_aseprite_tag* tag);
void pntr_draw_aseprite_tag(pntr_image* dst, pntr_aseprite_tag* tag, int posX, int posY);
void pntr_aseprite_set_tag_frame(pntr_aseprite_tag* tag, int frameNumber);
int pntr_aseprite_get_tag_frame(pntr_aseprite_tag* tag);
```

## Credits

- [George Sprite](https://opengameart.org/content/alternate-lpc-character-sprites-george) by [Sheep](https://opengameart.org/users/sheep)
- [cute_aseprite.h](https://github.com/RandyGaul/cute_headers/blob/master/cute_aseprite.h) by [RandyGaul](https://github.com/RandyGaul)

## License

Unless stated otherwise, all works are:

- Copyright (c) 2023 [Rob Loach](https://robloach.net)

... and licensed under:

- [zlib License](LICENSE)
