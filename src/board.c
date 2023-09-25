#include "optics.h"
#include "lib.h"

Mirrors *newmirrors();
Barriers *newbarriers();
void removemirror(Mirrors *mirrors, int i);
void removebarrier(Barriers *barriers, int i);

Board *newboard() {
    Board *board = malloc(sizeof(Board));
    board->mirrors = newmirrors();
    board->barriers = newbarriers();
    return board;
}

Mirrors *newmirrors() {
    Mirrors *mirrors = malloc(sizeof(Mirrors));
    mirrors->data = malloc(sizeof(Block) * MIRRORNUM);
    mirrors->n = 0;
    return mirrors;
}

Barriers *newbarriers() {
    Barriers *barriers = malloc(sizeof(Barriers));
    barriers->data = malloc(sizeof(Block) * BARRIERNUM);
    barriers->n = 1;

    Block midlinebarrier;
    midlinebarrier.x = ((float) WIDTH / 2.0) * SCALE;
    midlinebarrier.y = ((float) HEIGHT / 2.0) * SCALE - BLOCKSIZE / 2.0;
    midlinebarrier.width = 8 * SCALE;
    midlinebarrier.angle = 270;
    barriers->data[0] = midlinebarrier;

    return barriers;
}

void addmirror(Mirrors *mirrors, Block mirror) {
    if (mirrors->n == MIRRORNUM)
        return;
    mirrors->data[mirrors->n] = mirror;
    mirrors->n++;
}

void addbarrier(Barriers *barriers, Block barrier) {
    if (barriers->n == BARRIERNUM)
        return;
    barriers->data[barriers->n] = barrier;
    barriers->n++;
}

void removebarrier(Barriers *barriers, int i) {
    if (i < 1 || i >= barriers->n)
        return;
    int j;
    for (j = i; j < barriers->n - 1; j++)
        barriers->data[j] = barriers->data[j + 1];
    barriers->n--;
}

void removemirror(Mirrors *mirrors, int i) {
    if (i < 0 || i >= mirrors->n)
        return;
    int j;
    for (j = i; j < mirrors->n - 1; j++)
        mirrors->data[j] = mirrors->data[j + 1];
    mirrors->n--;
}

void removeblock(Board *board, Block *block) {
    if (block == NULL)
        return;
    if (
            block >= board->barriers->data &&
            block < board->barriers->data + board->barriers->n
        ) {
        removebarrier(board->barriers, block - board->barriers->data);
    } else {
        removemirror(board->mirrors, block - board->mirrors->data);
    }
}

void updateselectedblock(State *state) {
    if (state->selectedblock == NULL)
        return;

    /* handle middle locked barrier */
    if (state->selectedblock == state->board->barriers->data) {
        if (state->leftmb)
            state->selectedblock->x = state->mousepos->x;
        return;
    }
    
    /* update position */
    if (state->leftmb) {
        state->selectedblock->x = state->mousepos->x;
        state->selectedblock->y = state->mousepos->y;
    }

    /* update rotation */
    if (state->rightmb) {
        SDL_Point p1 = {state->selectedblock->x, state->selectedblock->y};
        SDL_Point p2 = {state->mousepos->x, state->mousepos->y};
        double angle = degatan(p2.y - p1.y, p2.x - p1.x);
        if (p2.x < p1.x)
            angle += 180;
        if (state->selectedside == LEFT)
            angle += 180;
        state->selectedblock->angle = angle;
    }
}


