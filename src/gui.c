#include "optics.h"
#include "lib.h"

#define SCALE 30
#define BORDERSIZE 8
#define WINWIDTH ((WIDTH * SCALE) + (BORDERSIZE * 2))
#define WINHEIGHT ((HEIGHT * SCALE) + (BORDERSIZE * 2))
#define BOARDWIDTH (WIDTH * SCALE)
#define BOARDHEIGHT (HEIGHT * SCALE)
#define FPS 60

void updatetexture(SDL_Context *ctx, State *state);
void rendermirrors(SDL_Context *ctx, Block *mirrors);
void renderblock(SDL_Context *ctx, Block block, SDL_Color color);
void rotvert(SDL_Vertex *v, double angle);

SDL_Context *SDL_InitContext() {
    SDL_Context *ctx = malloc(sizeof(SDL_Context));
    SDL_Window *window = SDL_CreateWindow("Optics", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, WINWIDTH,
                                          WINHEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_TARGET,
                                             BOARDWIDTH, BOARDHEIGHT);
    Uint32 lastupdate = SDL_GetTicks();
    SDL_Rect *dstrect = malloc(sizeof(SDL_Rect));
    dstrect->x = BORDERSIZE;
    dstrect->y = BORDERSIZE;
    dstrect->w = BOARDWIDTH;
    dstrect->h = BOARDHEIGHT;

    ctx->window = window;
    ctx->renderer = renderer;
    ctx->texture = texture;
    ctx->lastupdate = lastupdate;
    ctx->dstrect = dstrect;

    return ctx;
}

void render(SDL_Context *ctx, State *state) {
    ctx->lastupdate = SDL_GetTicks();
    SDL_SetRenderTarget(ctx->renderer, NULL);
    SDL_SetRenderDrawColor(ctx->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(ctx->renderer);

    updatetexture(ctx, state);
    SDL_SetRenderTarget(ctx->renderer, NULL);
    SDL_RenderCopy(ctx->renderer, ctx->texture, NULL, ctx->dstrect);

    SDL_RenderPresent(ctx->renderer);
}

void updatetexture(SDL_Context *ctx, State *state) {
    SDL_SetRenderTarget(ctx->renderer, ctx->texture);
    SDL_SetRenderDrawColor(ctx->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(ctx->renderer);

    rendermirrors(ctx, state->board->mirrors);
}

void rendermirrors(SDL_Context *ctx, Block *mirrors) {
    SDL_SetRenderTarget(ctx->renderer, ctx->texture);
    SDL_SetRenderDrawColor(ctx->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_Color color = {0xFF, 0x00, 0x00, 0xFF};
    int i;
    for (i = 0; i < MIRRORNUM; i++)
        renderblock(ctx, mirrors[i], color);
}

void renderblock(SDL_Context *ctx, Block block, SDL_Color color) {
    int x, y, w;
    x = block.x * SCALE;
    y = block.y * SCALE;
    w = ((float) block.width / 2.0) * SCALE;
    SDL_Vertex v1 = {{x - w - 10, y + 10}, color, {1, 1}};;
    SDL_Vertex v2 = {{x - w, y + 10}, color, {1, 1}};
    SDL_Vertex v3 = {{x - w, y}, color, {1, 1}};
    SDL_Vertex v4 = v2;
    SDL_Vertex v5 = {{x + w, y}, color, {1, 1}};
    SDL_Vertex v6 = v3;
    SDL_Vertex v7 = v4;
    SDL_Vertex v8 = {{x + w, y + 10}, color, {1, 1}};
    SDL_Vertex v9 = v5;
    SDL_Vertex v10 = v8;
    SDL_Vertex v11 = {{x + w + 10, y + 10}, color, {1, 1}};
    SDL_Vertex v12 = v9;
    SDL_Vertex vertices[] = {v1, v2, v3, v4, v5, v6, v7, v8, v9, v10,
                             v11, v12};

    int i;
    for (i = 0; i < 12; i++)
        rotvert(vertices + i, block.angle);

    SDL_RenderGeometry(ctx->renderer, NULL, vertices, 12, NULL, 0);
}

int needsupdate(Uint32 lastupdate) {
    return (SDL_GetTicks() - lastupdate) > (1000 / FPS);
}

void rotvert(SDL_Vertex *v, double angle) {
    float x = v->position.x;
    float y = v->position.y;
    v->position.x = (x * degcos(angle)) - (y * degsin(angle));
    v->position.y = (x * degsin(angle)) + (y * degcos(angle));
}
