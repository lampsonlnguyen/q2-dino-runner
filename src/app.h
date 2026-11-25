#ifndef Q2_DINO_APP_H
#define Q2_DINO_APP_H

#include "dinorunner.h"

enum { WIDTH = 375, HEIGHT = 320, SCENE_Y = 58, SCENE_HEIGHT = 150 };
typedef unsigned int u32;

struct app {
    struct dinorunner_s game;
    u32 *page;
    unsigned stride;
    unsigned long clock, best;
    int paused, quit, touch, save_pending, center_down;
    int ground_line_row;
};

int app_init(struct app *app, unsigned long best);
int app_sprite_y(const struct app *app, enum dinorunner_sprite_e sprite, int engine_y);
int app_input(struct app *app, const char *message);
void app_frame(struct app *app, u32 *page, unsigned stride, unsigned elapsed_ms);
int app_animated(const struct app *app);

#endif
