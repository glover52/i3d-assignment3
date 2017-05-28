#pragma once

#include "vec.h"
#include "mesh.h"
#include "animation.h"

typedef struct {
    Vec3f translation;
    Vec3f scale;
    Vec3f rotation;
    int angle;
    Mesh *mesh;
    unsigned int texture;
    Interpolator *interpolator;
} Model;

typedef struct node {
    Model *model;
    struct node *current_level;
    struct node *next_level;
} Node;

void render_tree(Node *node, DrawingFlags* flags);
void destroy_tree(Node *node);
void destroy_model(Model *model);
void update_tree(Node* node, float dt);
Node* create_tree(Model *model);
Model* create_model(Mesh *mesh);
