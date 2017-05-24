#include <string.h>
#include <stdbool.h>

#include "gl.h"
#include "util.h"
#include "state.h"
#include "player.h"
#include "level.h"
#include "osd.h"
#include "collision.h"

Globals globals;

static void cleanup() {
    destroyPlayer(&globals.player);
    destroyLevel(&globals.level);
}

static void updateKeyChar(unsigned char key, bool state) {
    switch (key) {
        case 'w':
            globals.controls.up = state;
            break;
        case 's':
            globals.controls.down = state;
            break;
        case 'a':
            globals.controls.left = state;
            break;
        case 'd':
            globals.controls.right = state;
            break;
        case ' ':
            globals.controls.jump = state;
        default:
            break;
    }
}

static void updateKeyInt(int key, bool state) {
    switch (key) {
        case GLUT_KEY_LEFT:
            globals.controls.turnLeft = state;
            break;
        case GLUT_KEY_RIGHT:
            globals.controls.turnRight = state;
            break;
        default:
            break;
    }
}

static void reshape(int width, int height) {
    glViewport(0,0, width, height);
    globals.camera.width = width;
    globals.camera.height = height;
    applyProjectionMatrix(&globals.camera);
}

static void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    applyViewMatrix(&globals.camera);

    static float lightPos[] = { 1, 1, 1, 0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    renderPlayer(&globals.player, &globals.drawingFlags);
    renderLevel(&globals.level, &globals.drawingFlags);

    renderOSD(&globals.osd);

    glutSwapBuffers();
}

static void die() {
    globals.osd.lives--;
    globals.player.jump = false;
    globals.player.pos = (Vec3f) { 0, 0, 4 };
    globals.player.initPos = (Vec3f) { 0, 0, 4 };
}

static bool isTooCurious(float value, float low, float high) {
    return value < low || value > high;
}

static bool isDrowning(Player* player, float low, float high) {
    float value = player->pos.z;
    return value > low
        && value < high
        && !player->jump
        && player->attachedTo == NULL
        && player->pos.y < 0.1;
}

static bool curiosityKilledTheCat() {
    Vec3f pos = globals.player.pos;
    float xlimit = globals.level.width / 2;
    float zlimit = globals.level.height / 2;
    float zriver = globals.level.river.pos.z;
    float height = globals.level.river.laneHeight;
    return isTooCurious(pos.x, -xlimit, xlimit)
        || isTooCurious(pos.z, -zlimit, zlimit)
        || isDrowning(&globals.player, zriver - height, zriver + height);
}

static void handleCollisions() {
    if (globals.player.attachedTo == NULL) {
        Level level = globals.level;
        Entity *log = detectCollisions(&globals.player, level.river.logs, level.river.numLanes);
        if (log != NULL && globals.player.pos.y > 0.1) {
            puts("Attached to log!");
            globals.player.attachedTo = log;
        } else {
            Entity *car = detectCollisions(&globals.player, level.road.enemies, level.road.numLanes);
            bool offLimits = curiosityKilledTheCat();
            if (car != NULL || offLimits) {
                printf("off limits: %d, car: %d\n", offLimits, car != NULL);
                die();
            }
        }
    }
}

static void update() {
    static int tLast = -1;

    if (tLast < 0)
        tLast = glutGet(GLUT_ELAPSED_TIME);

    int t = glutGet(GLUT_ELAPSED_TIME);
    int dtMs = t - tLast;
    float dt = (float)dtMs / 1000.0f;
    tLast = t;

    static int tLastFrame = 0;
    double dtFrame = (t - tLastFrame) / 1000.0f;
    if (dtFrame > globals.osd.frameRateInterval) {
        globals.osd.frameRate = globals.osd.frames / dtFrame;
        globals.osd.frames = 0;
        tLastFrame = t;
    }

    if (globals.halt) {
        return;
    }

    updateLevel(&globals.level, dt);
    handleCollisions();
    updatePlayer(&globals.player, dt, &globals.controls);

    globals.camera.pos = globals.player.pos;
    glutPostRedisplay();
}

static void keyDown(unsigned char key, int x, int y) {
    UNUSED(x);
    UNUSED(y);

    switch (key)
    {
        case 27:
        case 'q':
            cleanup();
            exit(EXIT_SUCCESS);
            break;
        case 'h':
            globals.halt = !globals.halt;
            if (globals.halt)
                printf("Stopping time\n");
            else
                printf("Resuming time\n");
            break;
        case 'l':
            globals.drawingFlags.lighting = !globals.drawingFlags.lighting;
            printf("Toggling lighting\n");
            break;
        case 't':
            globals.drawingFlags.textures = !globals.drawingFlags.textures;
            printf("Toggling textures\n");
            break;
        case 'n':
            globals.drawingFlags.normals = !globals.drawingFlags.normals;
            printf("Toggling normals\n");
            break;
        case 'o':
            globals.drawingFlags.axes = !globals.drawingFlags.axes;
            printf("Toggling axes\n");
            break;
        case 'p':
            globals.drawingFlags.wireframe = !globals.drawingFlags.wireframe;
            if (globals.drawingFlags.wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                printf("Using wireframe rendering\n");
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                printf("Using filled rendering\n");
            }
            break;
        case '+':
        case '=':
            globals.drawingFlags.segments = clamp(globals.drawingFlags.segments * 2, 8, 1024);
            generatePlayerGeometry(&globals.player, globals.drawingFlags.segments);
            generateLevelGeometry(&globals.level, globals.drawingFlags.segments);
            printf("Tesselation: %zu\n", globals.drawingFlags.segments);
            break;
        case '-':
            globals.drawingFlags.segments = clamp(globals.drawingFlags.segments / 2, 8, 1024);
            generatePlayerGeometry(&globals.player, globals.drawingFlags.segments);
            generateLevelGeometry(&globals.level, globals.drawingFlags.segments);
            printf("Tesselation: %zu\n", globals.drawingFlags.segments);
            break;
        default:
            updateKeyChar(key, true); // player controls are updated here and processed on each frame
            break;
    }

    glutPostRedisplay();
}

static void keyUp(unsigned char key, int x, int y) {
    UNUSED(x);
    UNUSED(y);

    updateKeyChar(key, false);
}

static void specialKeyDown(int key, int x, int y) {
    UNUSED(x);
    UNUSED(y);

    switch(key)
    {
    default:
        updateKeyInt(key, true);
        break;
    }

    glutPostRedisplay();
}

static void specialKeyUp(int key, int x, int y) {
    UNUSED(x);
    UNUSED(y);

    updateKeyInt(key, false);
}

static void mouseMotion(int x, int y) {
    int dX = x - globals.camera.lastX;
    int dY = y - globals.camera.lastY;

    if (globals.controls.lmb) {
        globals.camera.xRot += dX * 0.1;
        globals.camera.yRot += dY * 0.1;
        globals.camera.yRot = clamp(globals.camera.yRot, 0, 90);
    }

    if (globals.controls.rmb) {
        globals.camera.zoom += dY * 0.01;
        globals.camera.zoom = max(globals.camera.zoom, 0.5);
    }

    globals.camera.lastX = x;
    globals.camera.lastY = y;

    glutPostRedisplay();
}

static void mouseButton(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        globals.camera.lastX = x;
        globals.camera.lastY = y;
    }

    if (button == GLUT_LEFT_BUTTON) {
        globals.controls.lmb = state == GLUT_DOWN;
    }
    else if (button == GLUT_RIGHT_BUTTON) {
        globals.controls.rmb = state == GLUT_DOWN;
    }

    glutPostRedisplay();
}

static void init() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    globals.drawingFlags.segments = 8;
    globals.drawingFlags.wireframe = false;
    globals.drawingFlags.textures = true;
    globals.drawingFlags.lighting = true;
    initPlayer(&globals.player, &globals.drawingFlags);
    initLevel(&globals.level, &globals.drawingFlags);
    initCamera(&globals.camera);
    initOSD(&globals.osd);
    globals.camera.pos = globals.player.pos;
    globals.camera.width = 800;
    globals.camera.height = 600;
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("i3D Assignment 3");

    glutDisplayFunc(render);
    glutIdleFunc(update);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    glutMotionFunc(mouseMotion);
    glutMouseFunc(mouseButton);
    glutReshapeFunc(reshape);

    init();

    glutMainLoop();

    return EXIT_SUCCESS;
}
