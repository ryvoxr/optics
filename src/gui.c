#include "optics.h"

#define BOARDWIDTH (WIDTH * SCALE)
#define BOARDHEIGHT (HEIGHT * SCALE)
#define FPS 60

void updatetexture(SDL_Context *ctx, State *state);
void rendermirrors(SDL_Context *ctx, State *state);
void rendermirror(SDL_Context *ctx, Block mirro, SDL_Color color);
void renderbarriers(SDL_Context *ctx, State *state);
void renderbarrier(SDL_Context *ctx, Block barrier, SDL_Color color);

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

    // Render midline
    SDL_SetRenderDrawColor(ctx->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawLine(ctx->renderer, 0, BOARDHEIGHT / 2, BOARDWIDTH,
                       BOARDHEIGHT / 2);

    rendermirrors(ctx, state);
    renderbarriers(ctx, state);
}

void rendermirrors(SDL_Context *ctx, State *state) {
    Mirrors *mirrors = state->board->mirrors;
    SDL_Color defaultcolor = {0xFF, 0x00, 0x00, 0xFF};
    SDL_Color selectedcolor = {0x00, 0xFF, 0x00, 0xFF};
    int i;
    for (i = 0; i < mirrors->n; i++)
        if (mirrors->data + i == state->selectedblock)
            rendermirror(ctx, mirrors->data[i], selectedcolor);
        else
            rendermirror(ctx, mirrors->data[i], defaultcolor);
}

void rendermirror(SDL_Context *ctx, Block mirror, SDL_Color color) {
    int x, y, w;
    x = mirror.x;
    y = mirror.y;
    w = mirror.width / 2;
    SDL_Vertex v1 = {{-w - BLOCKSIZE, BLOCKSIZE}, color, {1, 1}};;
    SDL_Vertex v2 = {{-w, BLOCKSIZE}, color, {1, 1}};
    SDL_Vertex v3 = {{-w, 0}, color, {1, 1}};
    SDL_Vertex v4 = v2;
    SDL_Vertex v5 = {{w, 0}, color, {1, 1}};
    SDL_Vertex v6 = v3;
    SDL_Vertex v7 = v4;
    SDL_Vertex v8 = {{w, BLOCKSIZE}, color, {1, 1}};
    SDL_Vertex v9 = v5;
    SDL_Vertex v10 = v8;
    SDL_Vertex v11 = {{w + BLOCKSIZE, BLOCKSIZE}, color, {1, 1}};
    SDL_Vertex v12 = v9;
    SDL_Vertex vertices[] = {v1, v2, v3, v4, v5, v6, v7, v8, v9, v10,
                             v11, v12};

    int i;
    SDL_Vertex *vp;
    for (i = 0; i < 12; i++) {
        vp = vertices + i;
        rotvert(vp, mirror.angle);
        shiftvert(vp, x, y);
    }

    SDL_RenderGeometry(ctx->renderer, NULL, vertices, 12, NULL, 0);

    LineSegment front = mirrorfront(&mirror);
    LineSegment back = mirrorback(&mirror);
    SDL_SetRenderDrawColor(ctx->renderer, 0x00, 0x00, 0xFF, 0xFF);
    SDL_RenderDrawLine(ctx->renderer, front.p1.x, front.p1.y, front.p2.x,
                       front.p2.y);
    SDL_RenderDrawLine(ctx->renderer, back.p1.x, back.p1.y, back.p2.x,
                       back.p2.y);
}

void renderbarriers(SDL_Context *ctx, State *state) {
    Barriers *barriers = state->board->barriers;
    SDL_Color defaultcolor = {0x00, 0x00, 0x00, 0xFF};
    SDL_Color selectedcolor = {0x00, 0xFF, 0x00, 0xFF};
    int i;
    for (i = 0; i < barriers->n; i++) {
        if (state->selectedblock == barriers->data + i)
            renderbarrier(ctx, barriers->data[i], selectedcolor);
        else
            renderbarrier(ctx, barriers->data[i], defaultcolor);
    }
}

void renderbarrier(SDL_Context *ctx, Block barrier, SDL_Color color) {
    int x, y, w;
    x = barrier.x;
    y = barrier.y;
    w = barrier.width / 2;
    SDL_Vertex v1 = {{-w, BLOCKSIZE}, color, {1, 1}};
    SDL_Vertex v2 = {{w, 0}, color, {1, 1}};
    SDL_Vertex v3 = {{-w, 0}, color, {1, 1}};
    SDL_Vertex v4 = v1;
    SDL_Vertex v5 = {{w, BLOCKSIZE}, color, {1, 1}};
    SDL_Vertex v6 = v2;
    SDL_Vertex vertices[] = {v1, v2, v3, v4, v5, v6};

    int i;
    SDL_Vertex *vp;
    for (i = 0; i < 6; i++) {
        vp = vertices + i;
        rotvert(vp, barrier.angle);
        shiftvert(vp, x, y);
    }

    SDL_RenderGeometry(ctx->renderer, NULL, vertices, 6, NULL, 0);
}

int needsupdate(Uint32 lastupdate) {
    return (SDL_GetTicks() - lastupdate) > (1000 / FPS);
}

