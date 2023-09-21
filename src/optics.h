#include <SDL2/SDL.h>

#define WIDTH 56
#define HEIGHT 35

#define MIRRORSNUM 5
#define BARRIERSNUM 3

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Uint32 lastupdate;
    SDL_Rect *dstrect;
} SDL_Context;

typedef struct {
    int x;
    int y;
    long angle;
    int width;
} Block;

typedef struct {
    Block *mirrors;
    Block *barriers;
    int target;
} Board;

typedef struct {
    Board *board;
} State;

void cleanup(SDL_Context *ctx, State *state);

Board *newboard();

SDL_Context *SDL_InitContext();
void render(SDL_Context *ctx, State *state);
int needsupdate(Uint32 lastupdate);

void handleevents(SDL_Context *ctx, State *state);
