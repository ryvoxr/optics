#include "optics.h"

CollisionType generatelasersegment(State *state, LineSegment *segment, LineSegment *prevsegment, Block **prevblockp);
CollisionType intersectwithmirror(LineSegment *l1, Block *mirror, SDL_Point **collision);

void generatelaser(State *state, LineSegment **laserpath, int *laserlen) {
    int i;
    Block *prevblock = NULL;
    for (i = 0; i < MAXBOUNCES; i++) {
        LineSegment *newsegment = malloc(sizeof(LineSegment));
        LineSegment *prevsegment = i == 0 ? NULL : laserpath[i - 1];
        CollisionType collisiontype =  generatelasersegment(state, newsegment, prevsegment, &prevblock);
        printf("collisiontype: %d\n", collisiontype);

        /* maybe leaking hehe */
        laserpath[i] = newsegment;
        if (collisiontype == NONE || collisiontype == ABSORB) {
            i++;
            break;
        }
    }
    *laserlen = i;
}

/* generatelasersegment: generates the next laser segment and places it into
 * *segment. Fills *prevblock with a pointer to the reflecting block. Returns
 * CollisionType */
CollisionType generatelasersegment(State *state, LineSegment *segment, LineSegment *prevsegment, Block **prevblockp) {
    /* find trajectory  */
    LineSegment trajectory;
    if (prevsegment == NULL) {
        trajectory.p1.x = 1;
        trajectory.p1.y = ((HEIGHT * SCALE) / 2);
        trajectory.p2.x = WIDTH * SCALE;
        trajectory.p2.y = ((HEIGHT * SCALE) / 2);
    } else {
        // putchar('\n');
        Block *prevblock = *prevblockp;
        // printf("prevblock: x: %d, y %d, angle: %f\n", prevblock->x, prevblock->y, prevblock->angle);
        double tangent = prevblock->angle - 90;
        double incident = degatan(prevsegment->p2.y - prevsegment->p1.y,
                                  prevsegment->p2.x - prevsegment->p1.x) + 180;
        double reflection = incident + 2 * (tangent - incident);
        // printf("tan: %f, inc: %f, ref: %f\n", tangent, incident, reflection);
        trajectory.p1 = prevsegment->p2;
        trajectory.p2.y = reflection > 180 ? 0 : HEIGHT * SCALE;
        if (fmod(reflection, 90) == 0)
            trajectory.p2.x = trajectory.p1.x;
        else
            trajectory.p2.x = trajectory.p1.x + trajectory.p2.y * degtan(reflection);
        // printf("trajectory: (%d, %d) -> (%d, %d)\n", trajectory.p1.x, trajectory.p1.y, trajectory.p2.x, trajectory.p2.y);
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
            if (collision == NULL)
                continue;

            printf("wall collision: (%d, %d)\n", collision->x, collision->y);
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
    long x1, x2, x3, x4, y1, y2, y3, y4;
    x1 = l1.p1.x;
    x2 = l1.p2.x;
    x3 = l2.p1.x;
    x4 = l2.p2.x;
    y1 = l1.p1.y;
    y2 = l1.p2.y;
    y3 = l2.p1.y;
    y4 = l2.p2.y;

    long d = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);
    if (d == 0)
        return NULL;
    SDL_Point *p = malloc(sizeof(SDL_Point));
    p->x = ((x1*y2 - y1*x2)*(x3 - x4) - (x1 - x2)*(x3*y4 - y3*x4))/d;
    p->y = ((x1*y2 - y1*x2)*(y3 - y4) - (y1 - y2)*(x3*y4 - y3*x4))/d;

    if (p->x <= MIN(x1, x2) || p->x >= MAX(x1, x2)) {
        if ((p->x == x1) && (p->x == x2))
            return p;
        free(p);
        return NULL;
    }
    if (p->x <= MIN(x3, x4) || p->x >= MAX(x3, x4)) {
        if ((p->x == x3) && (p->x == x4))
            return p;
        free(p);
        return NULL;
    }

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

