#pragma once

#include "vec.h"
#include "mesh.h"

typedef struct {
    Vec3f translation;
    Vec3f scale;
    Vec3f rotation;
    Mesh *mesh;
    unsigned int texture;
} Model;

typedef struct node {
    Model *model;
    struct node *current_level;
    struct node *next_level;
} Node;

void render_tree(Node *node, DrawingFlags* flags);
