/* SPDX-License-Identifier: GPL-3.0-only */
#include "app.h"
#include "sprites.inc"

/* Reference Chrome Dino horizon: about 63% down the Q2 landscape display. */
enum { CENTRE = 218, RETURN = 170 };
struct rect { int x, y, w, h; };

/* The sprite coordinates from Dino Runner's SDL demo, in engine order. */
#define SPRITE(name, x, y, w, h) [DINORUNNER_SPRITE_##name] = {x, y, w, h}
static const struct rect sprites[] = {
    SPRITE(CACTUSLARGE1,332,2,25,50), SPRITE(CACTUSLARGE2,357,2,50,50),
    SPRITE(CACTUSLARGE3,407,2,75,50), SPRITE(CACTUSSMALL1,228,2,17,35),
    SPRITE(CACTUSSMALL2,245,2,34,35), SPRITE(CACTUSSMALL3,279,2,51,35),
    SPRITE(PTERODACTYL1,134,2,46,40), SPRITE(PTERODACTYL2,180,2,46,40),
    SPRITE(CLOUD,86,2,46,14),
    SPRITE(HORIZON1,2,54,WIDTH,12), SPRITE(HORIZON2,602,54,WIDTH,12),
    SPRITE(MOON1,624,2,20,40), SPRITE(MOON2,604,2,20,40), SPRITE(MOON3,584,2,20,40),
    SPRITE(MOON4,544,2,40,40), SPRITE(MOON5,524,2,20,40), SPRITE(MOON6,504,2,20,40),
    SPRITE(MOON7,484,2,20,40),
    SPRITE(RESTART,2,2,36,32), SPRITE(TEXT,655,2,191,11),
    SPRITE(TREX_STANDING1,848,2,44,47), SPRITE(TREX_STANDING2,892,2,44,47),
    SPRITE(TREX_RUNNING1,936,2,44,47), SPRITE(TREX_RUNNING2,980,2,44,47),
    SPRITE(TREX_CRASHED,1068,2,44,47), SPRITE(TREX_DUCKING1,1112,2,59,47),
    SPRITE(TREX_DUCKING2,1171,2,59,47), SPRITE(TREX_JUMPING,848,2,44,47),
    SPRITE(STAR1,645,2,8,9), SPRITE(STAR2,645,11,8,9), SPRITE(STAR3,645,20,8,9),
    SPRITE(0,654,2,10,13), SPRITE(1,664,2,10,13), SPRITE(2,674,2,10,13),
    SPRITE(3,684,2,10,13), SPRITE(4,694,2,10,13), SPRITE(5,704,2,10,13),
    SPRITE(6,714,2,10,13), SPRITE(7,724,2,10,13), SPRITE(8,734,2,10,13),
    SPRITE(9,744,2,10,13), SPRITE(HI,754,2,20,13), SPRITE(GAMEOVER,654,15,191,11)
};
#undef SPRITE

/* Find the horizontal ink row inside the Chrome horizon sprite. */
static int horizon_ink_row(void)
{
    const struct rect horizon = sprites[DINORUNNER_SPRITE_HORIZON1];
    int best_row = 0, best_ink = -1;
    for (int y = 0; y < horizon.h; ++y) {
        int ink = 0;
        for (int x = 0; x < horizon.w; ++x) {
            const unsigned index = (unsigned)(horizon.y + y) * SHEET_WIDTH +
                                   (unsigned)(horizon.x + x);
            ink += ((sheet[index / 2] >> (4 * (index % 2))) & 15u) != 0;
        }
        if (ink > best_ink) {
            best_ink = ink;
            best_row = y;
        }
    }
    return best_row;
}

/* Place the visible horizon at the T-Rex's *engine* ground level.
 * The engine's horizon starts at scene y=150, five pixels below the
 * standing T-Rex's feet (y=145). Moving the T-Rex down to meet that line
 * caused the reported floating/low-ground appearance. Keep the dinosaur
 * and its jump/collision coordinates unchanged; raise the horizon ink
 * instead. Cacti sit on the same line with a small visual-only offset. */
int app_sprite_y(const struct app *app, enum dinorunner_sprite_e sprite, int engine_y)
{
    if ((unsigned)sprite >= sizeof sprites / sizeof sprites[0])
        return engine_y;
    const int ground = app->game.trex.ground_y_pos + DINORUNNER_CONFIG_TREX_HEIGHT;
    if (sprite == DINORUNNER_SPRITE_HORIZON1 ||
        sprite == DINORUNNER_SPRITE_HORIZON2)
        return engine_y + ground - app->game.horizon.horizon_line.y_pos -
               app->ground_line_row;
    if (sprite >= DINORUNNER_SPRITE_CACTUSLARGE1 &&
        sprite <= DINORUNNER_SPRITE_CACTUSSMALL3)
        return ground - sprites[sprite].h;
    return engine_y;
}

static u32 *pixel(const struct app *app, int x, int y)
{
    return app->page + x * app->stride + HEIGHT - 1 - y;
}

static u32 gray(unsigned shade)
{
    return 0xff000000u | shade * 0x010101u;
}

static unsigned long timestamp(void *context)
{
    return ((struct app *)context)->clock;
}

static unsigned char clear(void *context)
{
    struct app *app = context;
    if (!app->page)
        return 1;
    u32 background = gray(app->game.inverted ? 13 : 242);
    for (int x = 0; x < WIDTH; ++x) {
        u32 *row = app->page + x * app->stride;
        for (int y = 0; y < HEIGHT; ++y)
            row[y] = background;
    }
    return 1;
}

static unsigned char draw(enum dinorunner_sprite_e sprite, const struct pos_s *pos,
                          unsigned char opacity, void *context)
{
    struct app *app = context;
    if (!app->page || !opacity)
        return 1;
    if ((unsigned)sprite >= sizeof sprites / sizeof sprites[0])
        return 0;
    struct rect source = sprites[sprite];
    int x0 = pos->x, y0 = app_sprite_y(app, sprite, pos->y) + SCENE_Y;
    int left = x0 < 0 ? -x0 : 0;
    int right = x0 + source.w > WIDTH ? WIDTH - x0 : source.w;
    int top = y0 < SCENE_Y ? SCENE_Y - y0 : 0;
    int bottom = y0 + source.h > SCENE_Y + SCENE_HEIGHT + 12 ?
                 SCENE_Y + SCENE_HEIGHT + 12 - y0 : source.h;
    for (int x = left; x < right; ++x) {
        for (int y = top; y < bottom; ++y) {
            unsigned index = (source.y + y) * SHEET_WIDTH + source.x + x;
            unsigned shade_index = (sheet[index / 2] >> (4 * (index % 2))) & 15u;
            if (!shade_index)
                continue;
            unsigned shade = palette[shade_index];
            if (app->game.inverted)
                shade = 255 - shade;
            u32 *dst = pixel(app, x0 + x, y0 + y);
            if (opacity < 255)
                shade = (shade * opacity + (*dst & 255u) * (255 - opacity)) / 255;
            *dst = gray(shade);
        }
    }
    return source.w != 0;
}

/* Five-by-seven capitals for the title and input hints. */
static const unsigned char glyphs[26][7] = {
    {14,17,17,31,17,17,17}, {30,17,17,30,17,17,30}, {14,17,16,16,16,17,14},
    {30,17,17,17,17,17,30}, {31,16,16,30,16,16,31}, {31,16,16,30,16,16,16},
    {14,17,16,23,17,17,15}, {17,17,17,31,17,17,17}, {14,4,4,4,4,4,14},
    {1,1,1,1,17,17,14}, {17,18,20,24,20,18,17}, {16,16,16,16,16,16,31},
    {17,27,21,21,17,17,17}, {17,25,21,19,17,17,17}, {14,17,17,17,17,17,14},
    {30,17,17,30,16,16,16}, {14,17,17,17,21,18,13}, {30,17,17,30,20,18,17},
    {15,16,16,14,1,1,30}, {31,4,4,4,4,4,4}, {17,17,17,17,17,17,14},
    {17,17,17,17,17,10,4}, {17,17,17,21,21,21,10}, {17,17,10,4,10,17,17},
    {17,17,10,4,4,4,4}, {31,1,2,4,8,16,31}
};

static void label(struct app *app, const char *text, int y, int scale)
{
    int length = 0;
    while (text[length])
        ++length;
    int x = (WIDTH - (length * 6 - 1) * scale) / 2;
    u32 color = gray(app->game.inverted ? 230 : 58);
    for (; *text; ++text, x += 6 * scale) {
        if (*text < 'A' || *text > 'Z')
            continue;
        const unsigned char *glyph = glyphs[*text - 'A'];
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (glyph[row] & (16u >> col))
                    for (int dy = 0; dy < scale; ++dy)
                        for (int dx = 0; dx < scale; ++dx)
                            *pixel(app, x + col * scale + dx, y + row * scale + dy) = color;
            }
        }
    }
}

static unsigned char read_score(unsigned long *best, void *context)
{
    *best = ((struct app *)context)->best;
    return 1;
}

static unsigned char write_score(unsigned long best, void *context)
{
    struct app *app = context;
    app->best = best;
    app->save_pending = 1;
    return 1;
}

int app_init(struct app *app, unsigned long best)
{
    *app = (struct app){.best = best};
    app->ground_line_row = horizon_ink_row();
    const struct dimension_s dimensions = {WIDTH, SCENE_HEIGHT};
    const struct dinorunner_callbacks_s callbacks = {
        .gettimestamp = timestamp, .clearcanvas = clear, .draw = draw,
        .readhighscore = read_score, .writehighscore = write_score
    };
    return dinorunner_init(&app->game, &dimensions, &callbacks, app);
}

static void jump(struct app *app)
{
    if (app->paused)
        app->paused = 0;
    else
        dinorunner_onkeyup(&app->game);
}

static int decimal(const char **message)
{
    const char *p = *message;
    while (*p == ' ')
        ++p;
    if (*p < '0' || *p > '9')
        return -1;
    int value = 0;
    while (*p >= '0' && *p <= '9' && value < 1000)
        value = value * 10 + *p++ - '0';
    *message = p;
    return *p >= '0' && *p <= '9' ? -1 : value;
}

int app_input(struct app *app, const char *message)
{
    if (!message[0] || message[1] != ' ' || !message[2] || message[3] != ' ')
        return 0;
    const char *args = message + 4;
    int a = decimal(&args);
    if (message[0] == 'k') {
        if (a == CENTRE) {
            if (message[2] == 'd') {
                if (app->center_down)
                    return 0;
                app->center_down = 1;
                jump(app); /* Jump on press, not release. */
                return 1;
            }
            if (message[2] == 'u') {
                if (app->center_down) {
                    app->center_down = 0;
                    return 0; /* Already handled this press. */
                }
                jump(app); /* Fallback for release-only input sources. */
                return 1;
            }
            return 0;
        }
        if (a != RETURN || message[2] != 'u')
            return 0;
        if (app->paused)
            app->quit = 1;
        else {
            if (app->touch)
                dinorunner_onkeynone(&app->game);
            app->touch = 0;
            app->paused = 1;
        }
        return 1;
    }
    if (message[0] != 't')
        return 0;
    if (message[2] == 'u' && app->touch) {
        dinorunner_onkeynone(&app->game);
        app->touch = 0;
        return 1;
    }
    int y = decimal(&args);
    if (message[2] != 'd' || a < 0 || a >= WIDTH || y < 0 || y >= HEIGHT)
        return 0;
    if (app->paused || !app->game.playing || y < SCENE_Y + SCENE_HEIGHT) {
        jump(app);
    } else {
        dinorunner_onkeydown(&app->game);
        app->touch = 1;
    }
    return 1;
}

void app_frame(struct app *app, u32 *page, unsigned stride, unsigned elapsed_ms)
{
    app->page = page;
    app->stride = stride;
    if (!app->paused)
        app->clock += elapsed_ms;
    dinorunner_update(&app->game);
    label(app, app->paused ? "PAUSED" : "DINO RUNNER", 29, 3);
    if (app->paused) {
        label(app, "CENTRE RESUME", 267, 2);
        label(app, "RETURN QUIT", 289, 2);
    } else {
        label(app, app->game.crashed ? "CENTRE RESTART" : "CENTRE JUMP", 257, 2);
        label(app, "HOLD HERE TO DUCK", 281, 2);
    }
    app->page = 0;
}

int app_animated(const struct app *app)
{
    return !app->paused && app->game.playing;
}
