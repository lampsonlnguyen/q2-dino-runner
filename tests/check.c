#include "app.h"

#include <assert.h>
#include <stdio.h>

enum { STRIDE = HEIGHT + 3 };
static u32 pixels[WIDTH * STRIDE];

int main(void)
{
    struct app app;
    assert(app_init(&app, 0));
    /* The visible horizon must meet the original T-Rex ground level.
     * Keep the dinosaur's engine position untouched, including jumping. */
    const int ground_line = app.game.trex.ground_y_pos + DINORUNNER_CONFIG_TREX_HEIGHT;
    assert(app.ground_line_row >= 0 && app.ground_line_row < 12);
    assert(ground_line == 145);
    /* Reference screenshot horizon at ~63% of image height. */
    assert(SCENE_Y + ground_line == 203);
    assert((SCENE_Y + ground_line) * 100 / HEIGHT >= 62);
    assert((SCENE_Y + ground_line) * 100 / HEIGHT <= 64);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_HORIZON1,
                        app.game.horizon.horizon_line.y_pos) +
               app.ground_line_row == ground_line);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_HORIZON2,
                        app.game.horizon.horizon_line.y_pos) +
               app.ground_line_row == ground_line);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_TREX_STANDING1,
                        app.game.trex.ground_y_pos) == app.game.trex.ground_y_pos);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_TREX_DUCKING1,
                        app.game.trex.ground_y_pos) == app.game.trex.ground_y_pos);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_TREX_JUMPING,
                        app.game.trex.ground_y_pos - 12) ==
               app.game.trex.ground_y_pos - 12);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_CACTUSSMALL1, 105) + 35 ==
           ground_line);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_CACTUSSMALL3, 105) + 35 ==
           ground_line);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_CACTUSLARGE1, 90) + 50 ==
           ground_line);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_CACTUSLARGE3, 90) + 50 ==
           ground_line);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_PTERODACTYL1, 50) == 50);
    assert(app_sprite_y(&app, DINORUNNER_SPRITE_CLOUD, 30) == 30);
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

    /* Press must jump immediately; release must not jump again. */
    assert(app_input(&app, "k d 218") && app.game.trex.jumping && app_animated(&app));
    float takeoff_velocity = app.game.trex.jump_velocity;
    assert(!app_input(&app, "k d 218")); /* Ignore key repeat. */
    assert(!app_input(&app, "k u 218"));
    assert(app.game.trex.jumping && app.game.trex.jump_velocity == takeoff_velocity);
    /* The engine counts completed jumps at landing, not at takeoff. */
    assert(app.game.trex.jump_count == 0);
    int ground = app.game.trex.y_pos;
    for (int i = 0; i < 5; ++i)
        app_frame(&app, pixels, STRIDE, 17);
    assert(app.game.trex.y_pos < ground);
    assert(app_input(&app, "k u 170") && app.paused);
    unsigned long paused_at = app.clock;
    app_frame(&app, pixels, STRIDE, 1000);
    assert(app.clock == paused_at && !app_animated(&app));
    assert(app_input(&app, "k d 218") && !app.paused);
    assert(!app_input(&app, "k u 218"));

    for (int i = 0; app.game.trex.jumping && i < 120; ++i)
        app_frame(&app, pixels, STRIDE, 17);
    assert(!app.game.trex.jumping && app.game.trex.jump_count == 1);
    assert(app_input(&app, "t d 185 285") && app.game.trex.ducking);
    assert(app_input(&app, "t u 185 285") && !app.game.trex.ducking);
    for (int i = 0; !app.game.crashed && i < 1200; ++i)
        app_frame(&app, pixels, STRIDE, 17);
    assert(app.game.crashed && app.save_pending && app.best && !app_animated(&app));
    unsigned long best = app.best;
    assert(app_input(&app, "k d 218") && !app.game.crashed && app_animated(&app));
    assert(!app_input(&app, "k u 218"));
    assert(app.best == best);
    assert(app_input(&app, "k u 170") && app.paused);
    assert(app_input(&app, "k u 170") && app.quit);
    assert(app_init(&app, best));
    assert(app.game.distance_meter.high_score == best);
    /* Release-only firmware/emulators still work. */
    assert(app_init(&app, best));
    assert(app_input(&app, "k u 218") && app.game.trex.jumping);
    assert(!app.center_down);
    assert(app_init(&app, best));
    puts("Dino Runner rendering and gameplay checks passed");
}
