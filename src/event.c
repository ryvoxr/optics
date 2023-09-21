#include "optics.h"

void handleevents(SDL_Context *ctx, State *state) {
    SDL_Event *event = malloc(sizeof(SDL_Event));
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                cleanup(ctx, state);
                break;
        }
    }
}
