#include "optics.h"
#include "lib.h"

void updatemousepos(SDL_Event *event, SDL_Point *mousepos);
void mirrorclick(State *state);
void barrierclick(State *state);
void updateselection(SDL_Event *event, State *state);
void mbdown(SDL_Event *event, SDL_Context *ctx, State *state);
void mbup(SDL_Event *event,  State *state);
void handleevent(SDL_Event *event, SDL_Context *ctx, State *state);
void clearmouseevents(SDL_Context *ctx, State *state);

void handleevents(SDL_Context *ctx, State *state) {
    SDL_Event *event = malloc(sizeof(SDL_Event));
    while (SDL_PollEvent(event)) {
        handleevent(event, ctx, state);
    }
}

void handleevent(SDL_Event *event, SDL_Context *ctx, State *state) {
    switch (event->type) {
        case SDL_QUIT:
            cleanup(ctx, state);
            break;
        case SDL_MOUSEMOTION:
            updatemousepos(event, state->mousepos);
            if (!(state->leftmb | state->rightmb))
                updateselection(event, state);
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_m:
                    mirrorclick(state);
                    break;
                case SDLK_b:
                    barrierclick(state);
                    break;
                case SDLK_d:
                    removeblock(state->board, state->selectedblock);
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            mbdown(event, ctx, state);
            break;
        case SDL_MOUSEBUTTONUP:
            mbup(event, state);
            break;
    }
}

/* clearmouseevents: handle the event stream but ignore all mouse events; call
 * this to remove jerkiness when using SDL_WarpMouseInWindow */
void clearmouseevents(SDL_Context *ctx, State *state) {
    SDL_Event *event = malloc(sizeof(SDL_Event));
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_MOUSEMOTION)
            continue;
        handleevent(event, ctx, state);
    }
}

void mbdown(SDL_Event *event, SDL_Context *ctx, State *state) {
    Block *selected  = state->selectedblock;
    switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            if (selected == NULL)
                break;
            if (!state->rightmb) {
                state->leftmb = 1;
                SDL_WarpMouseInWindow(NULL, selected->x + BORDERSIZE, selected->y + BORDERSIZE);
                clearmouseevents(ctx, state);
            }
            break;
        case SDL_BUTTON_RIGHT:
            if (selected == NULL)
                break;
            if (!state->leftmb) {
                state->rightmb = 1;
                int w = state->selectedblock->width / 2.0;
                int dx, dy;
                dx = w * degcos(selected->angle);
                dy = w * degsin(selected->angle);
                SDL_Point rightp = {selected->x + dx, selected->y + dy};
                SDL_Point leftp = {selected->x - dx, selected->y - dy};

                if (distance(state->mousepos, &rightp) < distance(state->mousepos, &leftp)) {
                    state->selectedside = RIGHT;
                    SDL_WarpMouseInWindow(NULL, rightp.x + BORDERSIZE, rightp.y + BORDERSIZE);
                } else {
                    state->selectedside = LEFT;
                    SDL_WarpMouseInWindow(NULL, leftp.x + BORDERSIZE, leftp.y + BORDERSIZE);
                }
                clearmouseevents(ctx, state);
            }
            break;
    }
}

void mbup(SDL_Event *event, State *state) {
    switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            state->leftmb = 0;
            break;
        case SDL_BUTTON_RIGHT:
            state->rightmb = 0;
            break;
    }
}

void updatemousepos(SDL_Event *event, SDL_Point *mousepos) {
    mousepos->x = event->motion.x - BORDERSIZE;
    mousepos->y = event->motion.y - BORDERSIZE;
}

void updateselection(SDL_Event *event, State *state) {
    int mindist = 5 * SCALE;
    Block *closest = NULL;

    int i;
    for (i = 0; i < state->board->mirrors->n; i++) {
        Block *mirror = &state->board->mirrors->data[i];
        SDL_Point p2 = {mirror->x, mirror->y};
        int dist = distance(state->mousepos, &p2);
        if (dist < mindist) {
            mindist = dist;
            closest = mirror;
        }
    }
    for (i = 0; i < state->board->barriers->n; i++) {
        Block *barrier = &state->board->barriers->data[i];
        SDL_Point p2 = {barrier->x, barrier->y};
        int dist = distance(state->mousepos, &p2);
        if (dist < mindist) {
            mindist = dist;
            closest = barrier;
        }
    }

    state->selectedblock = closest;
}

void mirrorclick(State *state) {
    Block mirror;
    mirror.x = state->mousepos->x;
    mirror.y = state->mousepos->y;
    mirror.width = 8 * SCALE;
    mirror.angle = 0;
    addmirror(state->board->mirrors, mirror);
}

void barrierclick(State *state) {
    Block barrier;
    barrier.x = state->mousepos->x;
    barrier.y = state->mousepos->y;
    barrier.width = 8 * SCALE;
    barrier.angle = 0;
    addbarrier(state->board->barriers, barrier);
}
