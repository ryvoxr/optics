#include "optics.h"

Block *newmirrors();
Block *newbarriers();

Board *newboard() {
    Board *board = malloc(sizeof(Board));
    board->mirrors = newmirrors();
    board->barriers = newbarriers();
    return board;
}

Block *newmirrors() {
    Block *mirrors = malloc(sizeof(Block) * MIRRORNUM);
    int i;
    Block mirror;
    for (i = 0; i < MIRRORNUM; i++) {
        mirrors[i].x = 0;
        mirrors[i].y = 0;
        mirrors[i].width = 0;
        mirrors[i].angle = 0;
    }
    return mirrors;
}

Block *newbarriers() {
    Block *barriers = malloc(sizeof(Block) * BARRIERNUM);
    int i;
    for (i = 0; i < BARRIERNUM; i++) {
        barriers[i].x = 0;
        barriers[i].y = 0;
        barriers[i].width = 0;
        barriers[i].angle = 0;
    }
    return barriers;
}
