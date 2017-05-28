#include "skybox.h"
#include "mesh.h"
#include "gl.h"

/*
 * Initialise the Skybox including loading all the texture for the specific
 * meshes. The initialisation is similar to that of creating a new Entity.
 * This needs a serious trim, probably by using a loop, but time is very tight
 * at the moment.
 */
void initSkybox(Skybox* skybox, Vec3f pos) {
    skybox->pos = pos;
    skybox->size = (Vec3f){ 10, 10, 10};

    skybox->wallMesh = createWall();

    skybox->walls = calloc(5, sizeof(Wall));
    Wall* wall = skybox->walls;

    wall->size = skybox->size;
    wall->pos = skybox->pos;
    wall->rot = (Vec3f) { 0, 0, 0 };
    wall->wallTexture = loadTexture("res/skybox/posx.jpg");
    wall++;

    wall->size = skybox->size;
    wall->pos = skybox->pos;
    wall->rot = (Vec3f) { 0, 90, 0 };
    wall->wallTexture = loadTexture("res/skybox/negz.jpg");
    wall++;

    wall->size = skybox->size;
    wall->pos = skybox->pos;
    wall->rot = (Vec3f) { 0, 180, 0 };
    wall->wallTexture = loadTexture("res/skybox/negx.jpg");
    wall++;

    wall->size = skybox->size;
    wall->pos = skybox->pos;
    wall->rot = (Vec3f) { 0, -90, 0 };
    wall->wallTexture = loadTexture("res/skybox/posz.jpg");
    wall++;

    wall->size = skybox->size;
    wall->pos = skybox->pos;
    wall->rot = (Vec3f) { -90, 0, -90 };
    wall->wallTexture = loadTexture("res/skybox/posy.jpg");
}

static void renderWall(Wall* wall, Mesh* mesh, DrawingFlags* flags) {
    glPushMatrix();
    glTranslatef(wall->pos.x, wall->pos.y, wall->pos.z);
    glRotatef(wall->rot.x, 1, 0, 0);
    glRotatef(wall->rot.y, 0, 1, 0);
    glRotatef(wall->rot.z, 0, 0, 1);
    glScalef(wall->size.x, wall->size.y, wall->size.z);
    glBindTexture(GL_TEXTURE_2D, wall->wallTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    bool lighting = flags->lighting;
    flags->lighting = false;
    renderMesh(mesh, flags);
    flags->lighting = lighting;
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

void updateSkybox(Skybox* skybox, Vec3f pos) {
    skybox->pos = pos;
}

void renderSkybox(Skybox* skybox, DrawingFlags* flags) {
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

    for(size_t i = 0; i < 5; ++i) {
        submitColor(WHITE);

        Wall* wall = skybox->walls + i;
        renderWall(wall, skybox->wallMesh, flags);
    }

    glPopAttrib();
}

void destroySkybox(Skybox* skybox) {
    free(skybox->walls);
    destroyMesh(skybox->wallMesh);
}
