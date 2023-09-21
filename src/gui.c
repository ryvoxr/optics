#include "optics.h"

#define SCALE 30
#define BORDERSIZE 8
#define WINWIDTH ((WIDTH * SCALE) + (BORDERSIZE * 2))
#define WINHEIGHT ((HEIGHT * SCALE) + (BORDERSIZE * 2))
#define BOARDWIDTH (WIDTH * SCALE)
#define BOARDHEIGHT (HEIGHT * SCALE)
#define FPS 60

void updatetexture(SDL_Context *ctx, State *state);

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
}

int needsupdate(Uint32 lastupdate) {
    return (SDL_GetTicks() - lastupdate) > (1000 / FPS);
}

