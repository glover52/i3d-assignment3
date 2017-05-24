#include "tree.h"
#include "mesh.h"
#include "gl.h"

void render_tree(Node *node, DrawingFlags* flags) {
    if (node == NULL) {
        return;
    }
    Model* model = node->model;
    if (model == NULL || model->mesh == NULL) {
        return;
    }

    glPushMatrix();

    glTranslatef(model->translation.x, model->translation.y, model->translation.z);
    glScalef(model->scale.x, model->scale.y, model->scale.z);
    glRotatef(model->angle, model->rotation.x, model->rotation.y, model->rotation.z);

    renderMesh(model->mesh, flags);

    render_tree(node->next_level, flags);

    glPopMatrix();

    render_tree(node->current_level, flags);
}

void destroy_tree(Node *node) {
    if (node == NULL) {
        return;
    }

    Model* model = node->model;
    if (model != NULL) {
        destroy_model(model);
    }

    destroy_tree(node->next_level);
    destroy_tree(node->current_level);
    free(node);
}

void destroy_model(Model *model) {
    if (model == NULL) {
        return;
    }
    if (model->mesh == NULL) {
        destroyMesh(model->mesh);
    }
    free(model);
}

Model* create_model(Mesh *mesh) {
    Vec3f none = {};
    Vec3f identity = {1, 1, 1};
    Model* model = malloc(sizeof(Model));
    model->mesh = mesh;
    model->translation = none;
    model->scale = identity;
    model->rotation = none;
    model->angle = 0;
    return model;
}

Node* create_tree(Model* model) {
    Node* node = malloc(sizeof(Node));
    node->model = model;
    node->current_level = NULL;
    node->next_level = NULL;
    return node;
}
