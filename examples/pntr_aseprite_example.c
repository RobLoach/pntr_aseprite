#define WIDTH 240
#define HEIGHT 135

#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_ASEPRITE_IMPLEMENTATION
#include "pntr_aseprite.h"

typedef struct AppData {
    pntr_aseprite* george;
    pntr_aseprite_tag* walk;
} AppData;

bool Init(pntr_app* app) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    appData->george = NULL;
    appData->walk = NULL;

    appData->george = pntr_load_aseprite("resources/george.aseprite");
    if (appData->george == NULL) {
        return false;
    }

    // Use the walk cycle that's authored in the file, along with its frame timings.
    appData->walk = pntr_load_aseprite_tag(appData->george, "Walk-Down");

    return appData->walk != NULL;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);

    pntr_clear_background(screen, PNTR_WHITE);

    // Progress the animation, and draw whichever frame it landed on.
    pntr_aseprite_update_tag(appData->walk, pntr_app_delta_time(app));

    pntr_draw_aseprite_tag(screen, appData->walk,
        WIDTH / 2 - pntr_aseprite_width(appData->george) / 2,
        HEIGHT / 2 - pntr_aseprite_height(appData->george) / 2
    );

    return true;
}

void Close(pntr_app* app) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    pntr_unload_aseprite_tag(appData->walk);
    pntr_unload_aseprite(appData->george);
}

pntr_app Main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    return (pntr_app) {
        .width = WIDTH,
        .height = HEIGHT,
        .title = "pntr_aseprite: Example",
        .init = Init,
        .update = Update,
        .close = Close,
        .fps = 60,
        .userData = pntr_load_memory(sizeof(AppData)),
    };
}
