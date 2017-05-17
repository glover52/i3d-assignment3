#include <string.h>
#include "osd.h"
#include "gl.h"
#include "util.h"

const int margin = 10;
const int line_height = 15;
const int col_width = 100;

void initOSD(OSD* osd) {
    osd->frameRate = 1.0;
    osd->frameRateInterval = 0.2;
    osd->frames = 0;
    osd->score = 0;
    osd->lives = 5;
}

static void renderText(const char* text, int x, int y) {
    glRasterPos2i(x, y);

    for (size_t i = 0; i < strlen(text); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
    }
}

static void renderLives(int lives) {
    int y = margin + line_height * 3;
    char buffer[40];

    snprintf(buffer, sizeof buffer, "%d", lives);

    submitColor(WHITE);
    renderText("Lives:", margin, y);
    renderText(buffer, margin + col_width, y);
}

static void renderScore(int score) {
    int y = margin + line_height * 2;
    char buffer[40];

    snprintf(buffer, sizeof buffer, "%d", score);

    submitColor(RED);
    renderText("Score:", margin, y);
    renderText(buffer, margin + col_width, y);
}

static void renderFrameRate(double frameRate) {
    int y = margin + line_height * 1;
    char buffer[40];

    snprintf(buffer, sizeof buffer, "%.0f / s", frameRate);

    submitColor(PURPLE);
    renderText("Frame rate:", margin, y);
    renderText(buffer, margin + col_width, y);
}

static void renderFramePeriod(double framePeriod) {
    int y = margin + line_height * 0;
    char buffer[40];

    snprintf(buffer, sizeof buffer, "%.0f ms", framePeriod);

    submitColor(CYAN);
    renderText("Frame time:", margin, y);
    renderText(buffer, margin + col_width, y);
}

void renderOSD(OSD *osd) {
    osd->frames++;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    renderScore(osd->score);
    renderLives(osd->lives);
    renderFrameRate(osd->frameRate);
    renderFramePeriod(1000.0 / osd->frameRate);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}
