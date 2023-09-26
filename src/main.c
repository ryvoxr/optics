#include <stdio.h>
#include <stdlib.h>
#include "optics.h"

int main() {
    State *state = malloc(sizeof(State));
    state->board = newboard();
    state->mousepos = malloc(sizeof(SDL_Point));
    state->selectedblock = NULL;
    state->leftmb = 0;
    state->rightmb = 0;

    SDL_Context *ctx = SDL_InitContext();

    SDL_Event event;
    while (1) {
        if (!needsupdate(ctx->lastupdate)) {
            SDL_Delay(1);
            continue;
        }
        handleevents(ctx, state);
        updateselectedblock(state);
        int laserlen = 0;
        LineSegment **laserpath = generatelaser(state, &laserlen);
        int i;
        for (i = 0; i < laserlen; i++) {
            printf("laserpath[%d]: (%d, %d) -> (%d, %d)\n", i, laserpath[i]->p1.x, laserpath[i]->p1.y, laserpath[i]->p2.x, laserpath[i]->p2.y);
        }
        render(ctx, state);
        SDL_RenderPresent(ctx->renderer);
    }
}

void cleanup(SDL_Context *ctx, State *state) {
    SDL_DestroyTexture(ctx->texture);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    free(ctx->dstrect);
    free(ctx);
    free(state->board->mirrors->data);
    free(state->board->mirrors);
    free(state->board->barriers->data);
    free(state->board->barriers);
    free(state->board);
    free(state);
    SDL_Quit();
    exit(0);
}


