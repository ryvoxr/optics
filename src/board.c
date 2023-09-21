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
    Block *mirrors = malloc(sizeof(Block) * MIRRORSNUM);
    int i;
    Block mirror;
    for (i = 0; i < MIRRORSNUM; i++) {
        mirror = mirrors[i];
        mirror.x = 0;
        mirror.y = 0;
        mirror.angle = 0;
    }
    return mirrors;
}

Block *newbarriers() {
    Block *barriers = malloc(sizeof(Block) * BARRIERSNUM);
    int i;
    Block barrier;
    for (i = 0; i < MIRRORSNUM; i++) {
        barrier = barriers[i];
        barrier.x = 0;
        barrier.y = 0;
        barrier.angle = 0;
    }
    return barriers;
}
