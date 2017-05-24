#include "collision.h"
#include <stdio.h>

bool detectCollision(Sphere *sphere1, Sphere *sphere2) {
    Vec3f centre_diff = subVec3f(sphere2->centre, sphere1->centre);
    float radius_sum = sphere1->radius + sphere2->radius;
    float d_2 = dotVec3f(centre_diff, centre_diff);

    if (d_2 < radius_sum * radius_sum)
        printf("d^2 < r^2! --- %.2f < %.2f\n", d_2, radius_sum * radius_sum);

    return d_2 < radius_sum * radius_sum;
}

Entity* detectCollisions(Player* player, Entity* entities, size_t num_entities) {
    for (size_t i = 0; i < num_entities; i++) {
        Entity* entity = entities + i;
        Sphere playerSphere = { player->pos, player->size };
        Sphere objectSphere = { entity->pos, magVec3f(entity->size) };

        if (detectCollision(&playerSphere, &objectSphere)) {
            printf("Detected a collision with %zu at (%.2f, %.2f, %.2f) and (%.2f, %.2f, %.2f)!\n",
                    i,
                    player->pos.x, player->pos.y, player->pos.z,
                    entity->pos.x, entity->pos.y, entity->pos.z);
            return entity;
        }
    }
    return NULL;
}
