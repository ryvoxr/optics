#include <SDL2/SDL.h>

#define WIDTH 56
#define HEIGHT 35
#define SCALE 30
#define BORDERSIZE 8
#define WINWIDTH ((WIDTH * SCALE) + (BORDERSIZE * 2))
#define WINHEIGHT ((HEIGHT * SCALE) + (BORDERSIZE * 2))

#define MIRRORNUM 5
#define BARRIERNUM 3
#define BLOCKTHICKNESS (1.0 / 3.0)
#define BLOCKSIZE ((int) (BLOCKTHICKNESS * SCALE))

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
    double angle;
    int width;
} Block;

typedef struct {
    Block *data;
    int n;
} Mirrors;

typedef struct {
    Block *data;
    int n;
} Barriers;

typedef struct {
    Mirrors *mirrors;
    Barriers *barriers;
    double target;
} Board;

typedef enum {
    RIGHT,
    LEFT,
} Side;

typedef struct {
    Board *board;
    SDL_Point *mousepos;
    Block *selectedblock;
    int leftmb;
    int rightmb;
    Side selectedside;
} State;

void cleanup(SDL_Context *ctx, State *state);

Board *newboard();
void addmirror(Mirrors *mirrors, Block mirror);
void addbarrier(Barriers *barriers, Block barrier);
void removeblock(Board *board, Block *block);
void updateselectedblock(State *state);

SDL_Context *SDL_InitContext();
void render(SDL_Context *ctx, State *state);
int needsupdate(Uint32 lastupdate);
int cmtopx(double cm);
double pxtocm(int px);
int distance(SDL_Point *p1, SDL_Point *p2);

void handleevents(SDL_Context *ctx, State *state);
