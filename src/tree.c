#include "tree.h"
#include "mesh.h"
#include "gl.h"

void render_tree(Node *node, DrawingFlags* flags) {
  if (node == NULL) {
      return;
  }

  glPushMatrix();

  Model *model = node->model;

  glTranslatef(model->translation.x, model->translation.y, model->translation.z);
  glScalef(model->scale.x, model->scale.y, model->scale.z);
  glRotatef(0.0, model->rotation.x, model->rotation.y, model->rotation.z);

  renderMesh(model->mesh, flags);

  render_tree(node->next_level, flags);

  glPopMatrix();

  render_tree(node->current_level, flags);
}
