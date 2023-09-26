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

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((b) > (a) ? (a) : (b))

#define MAXBOUNCES MIRRORNUM + 1
#define MAXCOLLISSIONS MAXBOUNCES + 1

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

typedef struct {
    SDL_Point p1;
    SDL_Point p2;
} LineSegment;

typedef enum {
    REFLECT,
    ABSORB,
    NONE,
} CollisionType;

void cleanup(SDL_Context *ctx, State *state);

/* board */
Board *newboard();
void addmirror(Mirrors *mirrors, Block mirror);
void addbarrier(Barriers *barriers, Block barrier);
void removeblock(Board *board, Block *block);
void updateselectedblock(State *state);
LineSegment mirrorfront(Block *mirror);
LineSegment mirrorback(Block *mirror);

/* gui */
SDL_Context *SDL_InitContext();
void render(SDL_Context *ctx, State *state);
int needsupdate(Uint32 lastupdate);

/* events */
void handleevents(SDL_Context *ctx, State *state);

/* geometry */
LineSegment **generatelaser(State *state, int *laserlen);
double slope(LineSegment line);
SDL_Point *intersect(LineSegment l1, LineSegment l2);
double degtorad(double deg);
double radtodeg(double rad);
double degsin(double deg);
double degcos(double deg);
double degtan(double deg);
double degatan(int y, int x);
int cmtopx(double cm);
double pxtocm(int px);
int distance(SDL_Point *p1, SDL_Point *p2);
void rotvert(SDL_Vertex *v, double angle);
void shiftvert(SDL_Vertex *v, int x, int y);
void rotpoint(SDL_Point *p, double angle);
void shiftpoint(SDL_Point *p, int x, int y);

