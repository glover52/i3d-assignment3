#pragma once

#include "util.h"
#include "controls.h"
#include "mesh.h"
#include "material.h"
#include "level.h"
#include "tree.h"

/*
 * Our player has position and velocity, which are set from the speed and rotation parameters
 * Gravity is also stored here just because our player is the only object affected by it and you might want to be able to adjust it on the fly
 */
typedef struct {
    Vec3f pos, vel, initPos, initVel;
    float speed, xRot, yRot, size, g;
    bool jump;
    Node* tree;
    Material material;
    Entity* attachedTo;
    unsigned int texture;
} Player;

void initPlayer(Player* player, DrawingFlags* flags);
void destroyPlayer(Player* player);
void generatePlayerGeometry(Player* player, size_t segments);
void updatePlayer(Player* player, float dt, Controls* controls);
void renderPlayer(Player* player, DrawingFlags* flags);
