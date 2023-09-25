#include <stdio.h>
#include <stdlib.h>
#include "optics.h"
#include "lib.h"

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
        render(ctx, state);
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


