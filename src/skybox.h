#pragma once
#include "mesh.h"

typedef struct {
    Vec3f pos, size;
    Vec3f rot;
    unsigned int wallTexture;
} Wall;

typedef struct {
    Vec3f size;
    Vec3f pos;
    Mesh* wallMesh;
    Wall* walls;
} Skybox;

void initSkybox(Skybox* skybox, Vec3f pos);
void renderSkybox(Skybox* skybox, DrawingFlags* flags);
void destroySkybox(Skybox* skybox);
