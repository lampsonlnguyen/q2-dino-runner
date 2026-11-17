#include "app.h"

#include <assert.h>
#include <stdio.h>

enum { STRIDE = HEIGHT + 3 };
static u32 pixels[WIDTH * STRIDE];

int main(void)
{
    struct app app;
    assert(app_init(&app, 0));
    app_frame(&app, pixels, STRIDE, 0);
    unsigned ink = 0;
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            assert(pixels[x * STRIDE + y] >> 24 == 255);
            ink += pixels[x * STRIDE + y] != 0xfff2f2f2u;
        }
        for (int y = HEIGHT; y < STRIDE; ++y)
            assert(pixels[x * STRIDE + y] == 0);
    }
    assert(ink > 1000 && !app_animated(&app));

    assert(app_input(&app, "k u 218") && app.game.trex.jumping && app_animated(&app));
    int ground = app.game.trex.y_pos;
    for (int i = 0; i < 5; ++i)
        app_frame(&app, pixels, STRIDE, 17);
    assert(app.game.trex.y_pos < ground);
    assert(app_input(&app, "k u 170") && app.paused);
    unsigned long paused_at = app.clock;
    app_frame(&app, pixels, STRIDE, 1000);
    assert(app.clock == paused_at && !app_animated(&app));
    assert(app_input(&app, "k u 218") && !app.paused);

    for (int i = 0; app.game.trex.jumping && i < 120; ++i)
        app_frame(&app, pixels, STRIDE, 17);
    assert(!app.game.trex.jumping);
    assert(app_input(&app, "t d 185 285") && app.game.trex.ducking);
    assert(app_input(&app, "t u 185 285") && !app.game.trex.ducking);
    for (int i = 0; !app.game.crashed && i < 1200; ++i)
        app_frame(&app, pixels, STRIDE, 17);
    assert(app.game.crashed && app.save_pending && app.best && !app_animated(&app));
    unsigned long best = app.best;
    assert(app_input(&app, "k u 218") && !app.game.crashed && app_animated(&app));
    assert(app.best == best);
    assert(app_input(&app, "k u 170") && app.paused);
    assert(app_input(&app, "k u 170") && app.quit);
    assert(app_init(&app, best));
    assert(app.game.distance_meter.high_score == best);
    puts("Dino Runner rendering and gameplay checks passed");
}
