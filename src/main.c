#include <stdio.h>
#include "optics.h"

int main() {
    State *state = malloc(sizeof(State));
    state->board = newboard();

    state->board->mirrors[0].x = 20;
    state->board->mirrors[0].y = 10;
    state->board->mirrors[0].angle = 45;
    state->board->mirrors[0].width = 6;
    state->board->mirrors[1].x = 10;
    state->board->mirrors[1].y = 20;
    state->board->mirrors[1].angle = 20;
    state->board->mirrors[1].width = 4;
    state->board->mirrors[2].x = 40;
    state->board->mirrors[2].y = 20;
    state->board->mirrors[2].angle = 0;
    state->board->mirrors[2].width = 8;

    SDL_Context *ctx = SDL_InitContext();

    SDL_Event event;
    while (1) {
        if (!needsupdate(ctx->lastupdate)) {
            SDL_Delay(1);
            continue;
        }
        handleevents(ctx, state);
        render(ctx, state);
    }
}

void cleanup(SDL_Context *ctx, State *state) {
    SDL_DestroyTexture(ctx->texture);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    free(ctx->dstrect);
    free(ctx);
    free(state->board->mirrors);
    free(state->board->barriers);
    free(state->board);
    free(state);
    SDL_Quit();
    exit(0);
}


