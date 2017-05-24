#include "level.h"
#include "gl.h"

/*
 * Initialize the road with all of the cars and the stuff we need to render them
 */
static void initRoad(Road* road, float laneWidth, float laneHeight, size_t numLanes, Vec3f pos, DrawingFlags* flags) {
    road->laneWidth = laneWidth;
    road->laneHeight = laneHeight;
    road->pos = pos;
    road->numLanes = numLanes;

    road->enemyMesh = createCube();
    road->enemyMaterial = (Material) { { 0.2, 0.2, 0.2, 1 }, { 1, 0, 0, 1 }, { 1, 1, 1, 1 }, 50 };

    // allocate and initialize all of our objects
    road->enemies = (Entity*) calloc(numLanes, sizeof(Entity));
    Entity* enemy = road->enemies;
    for (size_t i = 0; i < numLanes; ++i) {
        // position the object randomly along the width of the lane
        enemy->pos.x = getTRand(-5, 5);

        // position the object in its own lane so it doesn't collide with others
        enemy->pos.z = laneHeight / (float) numLanes * (float) i + road->pos.z;

        // objects in odd lanes travel in the opposite direction
        if (i % 2 == 0)
            enemy->vel.x = 0.5;
        else
            enemy->vel.x = -0.5;

        enemy->size = (Vec3f) { 0.15, 0.1, 0.1 };
        ++enemy;
    }

    road->terrainMesh = createPlane(laneWidth, laneHeight, flags->segments, flags->segments, false);
    road->terrainMaterial = (Material) { { 0.7, 0.7, 0.7, 1 }, { 1, 1, 1, 1 }, { 0.8, 0.8, 0.8, 1 }, 90 };
    road->terrainTexture = loadTexture("res/road.png");
}

/*
 * Same as above but for our river and logs
 */
static void initRiver(River* river, float laneWidth, float laneHeight, size_t numLanes, Vec3f pos, DrawingFlags* flags) {
    river->laneWidth = laneWidth;
    river->laneHeight = laneHeight;
    river->pos = pos;
    river->numLanes = numLanes;

    river->logMesh = createCylinder(flags->segments, flags->segments, 1);
    river->logMaterial = (Material) { { 0.2, 0.2, 0.2, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1 }, 40 };
    river->logTexture = loadTexture("res/wood.jpg");

    // allocate and initialize all of our objects
    river->logs = (Entity*) calloc(numLanes, sizeof(Entity));
    Entity* log = river->logs;
    for (size_t i = 0; i < numLanes; ++i) {
        // position the object randomly along the width of the lane
        log->pos.x = getTRand(-5, 5);

        // position the object in its own lane so it doesn't collide with others
        log->pos.z = laneHeight / (float) numLanes * (float) i + river->pos.z;

        // objects in odd lanes travel in the opposite direction
        if (i % 2 == 0)
            log->vel.x = 0.5;
        else
            log->vel.x = -0.5;

        // we specified our cylinders looking down the z axis so we need to make sure they are rotated the right way when we draw them
        log->rot.y = 90;
        log->size = (Vec3f) { 0.1, 0.1, 0.5 };
        ++log;
    }
    // Initialise the sand
    // river->terrainMesh = createPlane(laneWidth, laneHeight, flags->segments, flags->segments, true);
    // river->terrainMaterial = (Material) { { 1, 1, 1, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1 }, 90 };
    // river->terrainTexture = loadTexture("res/sand.jpg");

    // Initialise the river
    river->riverMesh = createPlane(laneWidth, laneHeight, flags->segments, flags->segments, false);
    river->riverMaterial = (Material) { { 1, 1, 1, RIVER_ALPHA }, { 1, 1, 1, RIVER_ALPHA }, { 0, 0, 1, RIVER_ALPHA }, 100 };
    river->riverTexture = loadTexture("res/water.jpg");
}

/*
 * Update an entity's position each frame and make sure it stays in the bounds specified
 */
static void updateEntity(Entity* entity, float minX, float maxX, float dt) {
    entity->pos = addVec3f(entity->pos, mulVec3f(entity->vel, dt));

    // make sure the object stays in bounds
    if (entity->pos.x < minX) {
        entity->pos.x = maxX;
    }
    else if (entity->pos.x > maxX) {
        entity->pos.x = minX;
    }
}

/*
 * Update all of our cars, we might want to add collision here as well later
 */
static void updateRoad(Road* road, float dt) {
    float maxX = road->laneWidth / 2.0 + road->pos.x;
    float minX = maxX - road->laneWidth;

    for (size_t i = 0; i < road->numLanes; ++i) {
        updateEntity(road->enemies + i, minX, maxX, dt);
    }
}

/*
 * The same as above for our logs
 */
static void updateRiver(River* river, float dt) {
    float maxX = river->laneWidth / 2.0 + river->pos.x;
    float minX = maxX - river->laneWidth;

    for (size_t i = 0; i < river->numLanes; ++i) {
        updateEntity(river->logs + i, minX, maxX, dt);
    }
}

/*
 * Render either a car or log object with the provided mesh
 */
static void renderEntity(Entity* entity, Mesh* mesh, DrawingFlags* flags) {
    glPushMatrix();
    glTranslatef(entity->pos.x, entity->pos.y, entity->pos.z);
    glRotatef(entity->rot.x, 1, 0, 0);
    glRotatef(entity->rot.y, 0, 1, 0);
    glScalef(entity->size.x, entity->size.y, entity->size.z);
    renderMesh(mesh, flags);
    glPopMatrix();
}

/*
 * Get the material and mesh for all of our cars and render them
 */
static void renderRoad(Road* road, DrawingFlags* flags) {
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

    glPushMatrix();
    glTranslatef(road->pos.x, 0.001, road->pos.z + 0.775f);
    glBindTexture(GL_TEXTURE_2D, road->terrainTexture);
    applyMaterial(&road->terrainMaterial);
    submitColor(GREY);
    renderMesh(road->terrainMesh, flags);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    for (size_t i = 0; i < road->numLanes; ++i) {

        applyMaterial(&road->enemyMaterial);
        submitColor(RED);
        renderEntity(road->enemies + i, road->enemyMesh, flags);
    }

    glPopAttrib();
}

/*
 * And the same as above for our logs
 */
static void renderRiver(River* river, DrawingFlags* flags) {
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

    // Log Rendering
    for (size_t i = 0; i < river->numLanes; ++i) {
        glBindTexture(GL_TEXTURE_2D, river->logTexture);
        applyMaterial(&river->logMaterial);
        glColor3f(0.65, 0.15, 0.15);
        renderEntity(river->logs + i, river->logMesh, flags);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Sand Rendering
//    glPushMatrix();
//    glTranslatef(river->pos.x, 0.001, river->pos.z - 0.775f);
//    glBindTexture(GL_TEXTURE_2D, river->terrainTexture);
//    applyMaterial(&river->terrainMaterial);
//    submitColor(YELLOW);
//    renderMesh(river->terrainMesh, flags);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    glPopMatrix();

    // River rendering
    glPushMatrix();
    glTranslatef(river->pos.x, 0.001f, river->pos.z + 0.775f);
    glBindTexture(GL_TEXTURE_2D, river->riverTexture);
    applyMaterial(&river->riverMaterial);
//    submitColor(BLUE);
    glColor4f(0, 1, 1, RIVER_ALPHA);
    renderMesh(river->riverMesh, flags);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    glPopAttrib();
}

/*
 * Cleanup the memory used by the road
 */
static void destroyRoad(Road* road) {
    free(road->enemies);
    destroyMesh(road->enemyMesh);
}

/*
 * Cleanup the memory used by the river
 */
static void destroyRiver(River* river) {
    free(river->logs);
    destroyMesh(river->logMesh);
}

/*
 * Generate the geometry used by the terrain and the logs.
 * Just done this way so we have an easy way to update the geometry after the tesselation is increased or decreased
 */
void generateLevelGeometry(Level* level, size_t segments) {
    if (level->terrainMesh)
        destroyMesh(level->terrainMesh);
    if (level->river.logMesh)
        destroyMesh(level->river.logMesh);

    level->terrainMesh = createPlane(level->width, level->height, segments, segments, true);
    level->river.logMesh = createCylinder(segments, segments, 1);
}

/*
 * Initialize all of the stuff we need for the game world
 */
void initLevel(Level* level, DrawingFlags* flags) {
    level->width = 10;
    level->height = 10;

    level->terrainMesh = createPlane(level->width, level->height, flags->segments, flags->segments, true);
    level->terrainMaterial = (Material) { { 0.2, 0.2, 0.2, 1 }, { 0, 1, 0, 1 }, { 0.3, 0.3, 0.3, 1 }, 20 };
    level->terrainTexture = loadTexture("res/grass.jpg");

    initRoad(&level->road, level->width, 1.75, 8, (Vec3f) { 0, 0, 1 }, flags);
    initRiver(&level->river, level->width, 1.75, 8, (Vec3f) { 0, 0, -3 }, flags);
}

/*
 * Cleanup the memory used by the game world
 */
void destroyLevel(Level* level) {
    destroyRoad(&level->road);
    destroyRiver(&level->river);
    destroyMesh(level->terrainMesh);
}

/*
 * Update the game state each frame
 */
void updateLevel(Level* level, float dt) {
    updateRoad(&level->road, dt);
    updateRiver(&level->river, dt);
}

/*
 * Render everything in the game world
 */
void renderLevel(Level* level, DrawingFlags* flags) {
    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, level->terrainTexture);
    applyMaterial(&level->terrainMaterial);
    submitColor(GREEN);
    renderMesh(level->terrainMesh, flags);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    glPopAttrib();

    renderRoad(&level->road, flags);
    renderRiver(&level->river, flags);
}
