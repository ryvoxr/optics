#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "lib.h"

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
