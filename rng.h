
#ifndef _RNG_H
#define _RNG_H

#include <cstdlib>
#include "vec3.h"

#include <iostream>

inline double rng() {
    return drand48();
}

inline vec3 random_in_unit_sphere() {
    vec3 point;
    do {
        point = 2.0f * vec3(rng(), rng(), rng()) - 1.0f;
    } while (point.length() >= 1.0f);
    return point;
}

inline vec3 random_in_unit_disk() {
    vec3 point;
    do {
        point = 2.0f * vec3(rng(), rng(), 0.5f) - 1.0f;
    } while (point.length() >= 1.0f);
    return point;
}

#endif /* _RNG_H */
