#define WIDTH 240
#define HEIGHT 135

#define PNTR_APP_IMPLEMENTATION
#include "pntr_app.h"

#define PNTR_ASEPRITE_IMPLEMENTATION
#include "pntr_aseprite.h"

typedef struct AppData {
    pntr_aseprite* george;
    float frame;
} AppData;

bool Init(pntr_app* app) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    appData->george = pntr_load_aseprite("resources/george.aseprite");

    return appData->george != NULL;
}

bool Update(pntr_app* app, pntr_image* screen) {
    AppData* appData = (AppData*)pntr_app_userdata(app);

    pntr_clear_background(screen, PNTR_WHITE);

    pntr_draw_aseprite(screen, appData->george, (int)appData->frame,
            WIDTH / 2 - pntr_aseprite_width(appData->george) / 2,
            HEIGHT / 2 - pntr_aseprite_height(appData->george) / 2);

    // Update the active frame
    appData->frame += pntr_app_delta_time(app) * 4.0f;
    if (appData->frame >= 4) {
        appData->frame = 0;
    }

    return true;
}

void Event(pntr_app* app, pntr_app_event* event) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    if (appData == NULL) {
        return;
    }

}

void Close(pntr_app* app) {
    AppData* appData = (AppData*)pntr_app_userdata(app);
    pntr_unload_aseprite(appData->george);
}

pntr_app Main(int argc, char* argv[]) {
    return (pntr_app) {
        .width = WIDTH,
        .height = HEIGHT,
        .title = "pntr_aseprite: Example",
        .init = Init,
        .update = Update,
        .close = Close,
        .event = Event,
        .fps = 60,
        .userData = pntr_load_memory(sizeof(AppData)),
    };
}
