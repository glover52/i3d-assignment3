#pragma once

#include <stdbool.h>
#include "vec.h"

typedef struct {
    Vec3f centre;
    float radius;
} Sphere;

bool detectCollision(Sphere *, Sphere *);
