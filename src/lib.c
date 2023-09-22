#include <math.h>
#include "lib.h"

double degtorad(double deg) {
    return deg * (M_PI / 180);
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
