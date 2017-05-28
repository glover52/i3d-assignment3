#include "player.h"
#include "gl.h"
#include "state.h"
#include "tree.h"
#include "util.h"

#define ROT_AMOUNT (M_PI / 4.0) // amount that rotation will change each frame the controls are pressed
#define SPEED_AMOUNT 1.0 // amount that speed will change each frame the controls are pressed


/*
 * Update the player's position and velocity with frametime dt
 */
static void integratePlayer(Player* player, float dt) {
    // update velocity with gravity
    player->vel.y -= player->g * dt;

    // this gives a pretty reasonable integration of acceleration with each step
    player->pos.y += player->vel.y * dt + 0.5f * player->g * dt * dt;
    player->pos.x += player->vel.x * dt;
    player->pos.z += player->vel.z * dt;

    // when we have finished jumping, reset for next jump
    if (player->pos.y <= 0) {
        player->jump = false;
        player->pos.y = 0;
        player->initPos = player->pos;
    }
}

/*
 * Initialise the player
 */
void initPlayer(Player* player, DrawingFlags* flags) {
    player->pos = (Vec3f) { 0, 0, 4 };
    player->initPos = player->pos;

    player->xRot = M_PI / 4.0;
    player->yRot = M_PI;
    player->speed = 2.0;
    player->size = 0.1;
    player->g = 9.8;
    player->jump = false;
    player->attachedTo = NULL;
    player->texture = loadTexture("res/frog.jpg");
    generatePlayerGeometry(player, flags->segments);
    player->material = (Material) { { 0.2, 0.2, 0.2, 1 }, { 0.1, 0.5, 0.9, 1 }, { 1, 1, 1, 1 }, 50 };
}

/*
 * Cleanup any memory used by the player
 */
void destroyPlayer(Player* player) {
    destroy_tree(player->tree);
}

static Model* createLeg(double x, double z, double rot) {
    Model* leg = create_model(createCube());
    leg->translation = (Vec3f) { x, 0, z};
    leg->scale = (Vec3f) { 0.3, 0.3, 0.3};
    leg->rotation = (Vec3f) { 0.0, 1.0, 0.0};
    leg->angle = rot;
    leg->interpolator = createInterpolator(3, 0.2);
    leg->interpolator->keyframes[0] = (Keyframe) {0.0, 0.0};
    leg->interpolator->keyframes[1] = (Keyframe) {0.1, -z};
    leg->interpolator->keyframes[2] = (Keyframe) {0.2, 0.0};
    leg->interpolator->attribute_to_update = &leg->rotation.x;

    return leg;
}

static Model* createThigh(double x, double z, double rot) {
    Model* leg = create_model(createCube());
    leg->translation = (Vec3f) { x, 0, z};
    leg->scale = (Vec3f) { 0.9, 0.9, 0.9};
    leg->rotation = (Vec3f) { 0.0, 0.0, 1.0};
    leg->angle = rot;
    leg->interpolator = createInterpolator(3, 0.2);
    leg->interpolator->keyframes[0] = (Keyframe) {0.0, 0.0};
    leg->interpolator->keyframes[1] = (Keyframe) {0.1, -z};
    leg->interpolator->keyframes[2] = (Keyframe) {0.2, 0.0};
    leg->interpolator->attribute_to_update = &leg->rotation.z;
    return leg;
}

static void addLegs(Node* body) {
    Node* leg1 = create_tree(createLeg(0.7, 0.7, -45.0));
    Node* leg2 = create_tree(createLeg(-0.7, -0.7, -45.0));
    Node* leg3 = create_tree(createLeg(0.7, -0.7, -45.0));
    Node* leg4 = create_tree(createLeg(-0.7, 0.7, -45.0));

    leg1->current_level = leg2;
    leg2->current_level = leg3;
    leg3->current_level = leg4;
    body->next_level = leg1;

    Node* thigh1 = create_tree(createThigh(1.0, 1.0, 0.0));
    Node* thigh2 = create_tree(createThigh(-1.0, -1.0, 0.0));
    Node* thigh3 = create_tree(createThigh(-1.0, -1.0, 0.0));
    Node* thigh4 = create_tree(createThigh(1.0, 1.0, 0.0));

    leg1->next_level = thigh1;
    leg2->next_level = thigh2;
    leg3->next_level = thigh3;
    leg4->next_level = thigh4;
}

static void addHead(Node* body, size_t segments) {
    Model* head = create_model(createCube());
    head->scale = (Vec3f) {0.4, 0.3, 0.4};
    head->translation = (Vec3f) {0.0, 0.7, 0.7};
    Node* headAssemply = create_tree(head);

    Model* eye1 = create_model(createCylinder(segments, segments, 0.3));
    eye1->translation = (Vec3f) {-0.5, 0.8, 1.0};
    eye1->rotation = (Vec3f) {1.0, 0.2, 0.0};
    eye1->angle = -30;

    Model* eye2 = create_model(createCylinder(segments, segments, 0.3));
    eye2->translation = (Vec3f) {0.5, 0.8, 1.0};
    eye2->rotation = (Vec3f) {1.0, -0.2, 0.0};
    eye2->angle = -30;

    headAssemply->next_level = create_tree(eye1);
    headAssemply->next_level->current_level = create_tree(eye2);

    body->current_level = headAssemply;

    head->interpolator = createInterpolator(5, 3.0);
    eye1->interpolator = createInterpolator(3, 0.6);
    eye2->interpolator = createInterpolator(3, 0.6);

    head->interpolator->keyframes[0] = (Keyframe) {0.0, 0.2};
    head->interpolator->keyframes[1] = (Keyframe) {1.3, 0.4};
    head->interpolator->keyframes[2] = (Keyframe) {2.0, 0.5};
    head->interpolator->keyframes[3] = (Keyframe) {2.7, 0.2};
    head->interpolator->keyframes[4] = (Keyframe) {3.0, 0.2};

    eye1->interpolator->keyframes[0] = (Keyframe) {0.0, 1.0};
    eye1->interpolator->keyframes[1] = (Keyframe) {0.3, 2.0};
    eye1->interpolator->keyframes[2] = (Keyframe) {0.6, 1.0};

    eye2->interpolator->keyframes[0] = (Keyframe) {0.0, 1.0};
    eye2->interpolator->keyframes[1] = (Keyframe) {0.3, 2.0};
    eye2->interpolator->keyframes[2] = (Keyframe) {0.6, 1.0};

    eye1->interpolator->attribute_to_update = &eye1->scale.z;
    eye2->interpolator->attribute_to_update = &eye2->scale.z;
    head->interpolator->attribute_to_update = &head->scale.y;
}

/*
 * Generate the player mesh, or regenerate it if it already exists.
 * This should be called whenever the tesselation is increased or decreased
 */
void generatePlayerGeometry(Player* player, size_t segments) {
    destroy_tree(player->tree);
    Model* body = create_model(createSphere(segments, segments));
    Node* tree = create_tree(body);
    addLegs(tree);
    addHead(tree, segments);

    player->tree = tree;
}

/*
 * Update the player's state for frametime dt.
 * If we aren't jumping, update speed and rotation from controls, otherwise animate the next step of our jump
 */
void updatePlayer(Player* player, float dt, Controls* controls) {
    if (!player->jump) {
        update_tree(player->tree->current_level, dt);

        // process controls
        if (controls->up) {
            player->speed += SPEED_AMOUNT * dt;
        }
        if (controls->down) {
            player->speed -= SPEED_AMOUNT * dt;
        }
        if (controls->left) {
            player->xRot += ROT_AMOUNT * dt;
        }
        if (controls->right) {
            player->xRot -= ROT_AMOUNT * dt;
        }
        if (controls->turnLeft) {
            player->yRot += ROT_AMOUNT * dt;
        }
        if (controls->turnRight) {
            player->yRot -= ROT_AMOUNT * dt;
        }
        if (controls->jump) {
            player->jump = true;
            player->attachedTo = NULL;
        }

        // dont let the player jump backwards, into the floor or over entire map
        player->speed = clamp(player->speed, 0, 2.75f);
        player->xRot = clamp(player->xRot, 0, (float) M_PI / 2.0f);
        player->yRot = clamp(player->yRot, (float) (M_PI * 0.5), (float) (M_PI + M_PI * 0.5));

        // set our initial velocity from speed and rotation
        player->initVel.x = cosf(player->xRot) * sinf(player->yRot);
        player->initVel.y = sinf(player->xRot);
        player->initVel.z = cosf(player->yRot) * cosf(player->xRot);

        player->initVel = mulVec3f(player->initVel, player->speed);
        player->vel = player->initVel;
    } else {
        update_tree(player->tree->next_level, dt);
        integratePlayer(player, dt);
    }

    if (player->attachedTo != NULL){
        player->jump = false;
        player->pos = addVec3f(player->attachedTo->pos, mulVec3f(player->attachedTo->vel, dt));
        player->pos.y = player->attachedTo->pos.y + player->attachedTo->size.y;
        player->initPos = player->pos;
    }
}

/*
 * Draw the player's mesh, as well as a parabola showing our jump arc and a visualization of our current velocity
 */
void renderPlayer(Player* player, DrawingFlags* flags) {
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

    // draw the parabola from the starting point of the jump
    glPushMatrix();
    glTranslatef(player->initPos.x, player->initPos.y, player->initPos.z);
    drawParabola(BLUE, player->initVel, player->g, flags);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(player->pos.x, player->pos.y, player->pos.z);

    // draw the player mesh at our current position
    glPushMatrix();
    glRotatef(RADDEG(player->yRot), 0, 1, 0);
    glScalef(player->size, player->size, player->size);
    applyMaterial(&player->material);
    glColor3f(0.1, 0.5, 0.9);
    glBindTexture(GL_TEXTURE_2D, player->texture);
    render_tree(player->tree, flags);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    // draw the visualization of the player's velocity at our current position
    glBegin(GL_LINES);
    drawLine(PURPLE, (Vec3f) { 0, 0, 0 }, mulVec3f(player->vel, 0.1));
    glEnd();
    glPopMatrix();

    glPopAttrib();
}
