#pragma once

typedef struct {
    double frameRate;
    double frameRateInterval;
    int frames;
    int score;
    int lives;
} OSD;

void initOSD(OSD* osd);
void renderOSD(OSD *osd);
