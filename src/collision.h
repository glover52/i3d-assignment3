#pragma once

#include <stdbool.h>
#include "vec.h"
#include "player.h"
#include "level.h"

typedef struct {
    Vec3f centre;
    float radius;
} Sphere;

bool detectCollision(Sphere *, Sphere *);
Entity* detectCollisions(Player* player, Entity* entities, size_t num_entities);
