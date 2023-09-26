#include "optics.h"

CollisionType generatelasersegment(State *state, LineSegment *segment, int i, Block **prevblockp);
CollisionType intersectwithmirror(LineSegment *l1, Block *mirror, SDL_Point **collision);

LineSegment **generatelaser(State *state, int *laserlen) {
    LineSegment **laserpath = malloc(sizeof(int) * MAXBOUNCES);

    int i = 0;
    Block *prevblock = NULL;
    for (i = 0; i < MAXBOUNCES; i++) {
        laserpath[i] = malloc(sizeof(LineSegment));
        CollisionType collisiontype =  generatelasersegment(state, laserpath[i], i, &prevblock);
        if (collisiontype == NONE || collisiontype == ABSORB) {
            i++;
            break;
        }
    }
    laserpath = reallocarray(laserpath, i, sizeof(int));
    *laserlen = i;
    return laserpath;
}

/* generatelasersegment: generates the next laser segment and places it into
 * *segment. Fills *prevblock with a pointer to the reflecting block. Returns
 * CollisionType */
CollisionType generatelasersegment(State *state, LineSegment *segment, int n, Block **prevblockp) {
    /* find trajectory  */
    LineSegment trajectory;
    if (n == 0) {
        trajectory.p1.x = 1;
        trajectory.p1.y = ((HEIGHT / 2.0) * SCALE);
        trajectory.p2.x = WIDTH * SCALE + 1;
        trajectory.p2.y = ((HEIGHT / 2.0) * SCALE);
    } else {
        printf("in else\n");
        Block *prevblock = *prevblockp;
        LineSegment *prevsegment = segment--;
        double tangent = fmod(prevblock->angle + 90, 360);
        double incident = degatan(prevsegment->p2.y - prevsegment->p1.y,
                                  prevsegment->p2.x - prevsegment->p1.x) +
                          prevblock->angle;
        double reflection = incident + 2 * (tangent - incident);
        trajectory.p1 = prevsegment->p2;
        trajectory.p2.y = reflection > 180 ? 0 : HEIGHT * SCALE;
        trajectory.p2.x = trajectory.p1.x + trajectory.p1.y * degtan(fmod(reflection, 90));
        printf("trajectory: (%d, %d) -> (%d, %d)\n", trajectory.p1.x, trajectory.p1.y, trajectory.p2.x, trajectory.p2.y);
    }

    /* find closest collision */
    Block *closestblock = NULL;
    SDL_Point *closestcollision = NULL;
    CollisionType closestcollisiontype = NONE;
    int mindist = WIDTH * HEIGHT * SCALE;
    int i;
    for (i = 0; i < state->board->mirrors->n; i++) {
        Block *block = state->board->mirrors->data + i;
        SDL_Point *collision = NULL;
        CollisionType collisiontype = intersectwithmirror(&trajectory, block, &collision);

        if (collisiontype == NONE)
            continue;

        int dist = distance(&trajectory.p1, collision);
        if (dist < mindist) {
            mindist = dist;
            closestblock = block;
            if (closestcollision != NULL)
                free(closestcollision);
            closestcollision = collision;
            closestcollisiontype = collisiontype;
        } else {
            free(collision);
        }
    }

    /* if no collision yet, must collide with wall */
    if (closestcollision == NULL) {
        LineSegment leftwall = {{0, 0}, {0, HEIGHT * SCALE}};
        LineSegment rightwall = {{WIDTH * SCALE, 0}, {WIDTH * SCALE, HEIGHT * SCALE}};
        LineSegment topwall = {{0, 0}, {WIDTH * SCALE, 0}};
        LineSegment bottomwall = {{0, HEIGHT * SCALE}, {WIDTH * SCALE, HEIGHT * SCALE}};
        LineSegment walls[] = {leftwall, rightwall, topwall, bottomwall};
        for (i = 0; i < 4; i++) {
            CollisionType collisiontype = ABSORB;
            SDL_Point *collision = intersect(trajectory, walls[i]);
            if (collision != NULL) {
                int min = MIN(trajectory.p1.x, trajectory.p2.x);
                int max = MAX(trajectory.p1.x, trajectory.p2.x);
                if (collision->x < min || collision->x > max) {
                    collision = NULL;
                }
            }
            if (collision == NULL)
                continue;

            int dist = distance(&trajectory.p1, collision);
            if (dist < mindist) {
                mindist = dist;
                closestblock = NULL;
                if (closestcollision != NULL)
                    free(closestcollision);
                closestcollision = collision;
                closestcollisiontype = ABSORB;
            } else {
                free(collision);
            }
        }
    }

    if (closestcollision == NULL) {
        return NONE;
    } else {
        segment->p1 = trajectory.p1;
        segment->p2 = *closestcollision;
        if (closestblock != NULL)
            *prevblockp = closestblock;

        return closestcollisiontype;
    }
}

/* intersectwithmirror: fills *collision with a pointer to the collision (NULL
 * if no collision). Returns CollisionType */
CollisionType intersectwithmirror(LineSegment *l1, Block *mirror, SDL_Point **collision) {
    LineSegment front = mirrorfront(mirror);
    LineSegment back = mirrorback(mirror);
    SDL_Point *frontcollision = intersect(*l1, front);
    SDL_Point *backcollision = intersect(*l1, back);

    if (frontcollision != NULL) {
        int minfront = MIN(front.p1.x, front.p2.x);
        int maxfront = MAX(front.p1.x, front.p2.x);
        if (frontcollision->x < minfront || frontcollision->x > maxfront) {
            frontcollision = NULL;
        }
    }
    if (backcollision != NULL) {
        int minback = MIN(back.p1.x, back.p2.x);
        int maxback = MAX(back.p1.x, back.p2.x);
        if (backcollision->x < minback || backcollision->x > maxback) {
            backcollision = NULL;
        }
    }

    if (frontcollision == NULL && backcollision == NULL)
        return NONE;

    int frontdist = frontcollision == NULL ? WIDTH * HEIGHT * SCALE : distance(&l1->p1, frontcollision);
    int backdist = backcollision == NULL ? WIDTH * HEIGHT * SCALE : distance(&l1->p1, backcollision);
    if (frontdist < backdist) {
        *collision = frontcollision;
        if (backcollision != NULL)
            free(backcollision);
        return REFLECT;
    } else if (backdist < frontdist) {
        *collision = backcollision;
        if (frontcollision != NULL)
            free(frontcollision);
        return ABSORB;
    } else {
        return NONE;
    }
}

double slope(LineSegment line) {
    int dy = line.p2.y - line.p1.y;
    int dx = line.p2.x - line.p1.x;
    return (double) dy / (double) dx;
}

/* intersect: returns a pointer to the intersection point of two lines, or NULL
 * if they don't intersect */
SDL_Point *intersect(LineSegment l1, LineSegment l2) {
    int x1, x2, x3, x4, y1, y2, y3, y4;
    x1 = l1.p1.x;
    x2 = l1.p2.x;
    x3 = l2.p1.x;
    x4 = l2.p2.x;
    y1 = l1.p1.y;
    y2 = l1.p2.y;
    y3 = l2.p1.y;
    y4 = l2.p2.y;

    int d = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);
    if (d == 0)
        return NULL;
    SDL_Point *p = malloc(sizeof(SDL_Point));
    p->x = ((x1*y2 - y1*x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3*x4))/d;
    p->y = ((x1*y2 - y1*x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3*x4))/d;

    return p;
}

int distance(SDL_Point *p1, SDL_Point *p2) {
    return sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2));
}

void rotpoint(SDL_Point *p, double angle) {
    float x = p->x;
    float y = p->y;
    p->x = (x * degcos(angle)) - (y * degsin(angle));
    p->y = (x * degsin(angle)) + (y * degcos(angle));
}

void rotvert(SDL_Vertex *v, double angle) {
    float x = v->position.x;
    float y = v->position.y;
    v->position.x = (x * degcos(angle)) - (y * degsin(angle));
    v->position.y = (x * degsin(angle)) + (y * degcos(angle));
}

void shiftpoint(SDL_Point *p, int x, int y) {
    p->x += x;
    p->y += y;
}

void shiftvert(SDL_Vertex *v, int x, int y) {
    v->position.x += x;
    v->position.y += y;
}

int cmtopx(double cm) {
    return cm * SCALE;
}

double pxtocm(int px) {
    return (double) px / SCALE;
}

double degtorad(double deg) {
    return deg * (M_PI / 180);
}

double radtodeg(double rad) {
    return rad * (180 / M_PI);
}

double degsin(double deg) {
    return sin(degtorad(deg));
}

double degcos(double deg) {
    return cos(degtorad(deg));
}

double degtan(double deg) {
    return tan(degtorad(deg));
}

double degatan(int y, int x) {
    return radtodeg(atan((float) y / (float) x));
}

