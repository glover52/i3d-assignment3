#pragma once

typedef struct {
    double frameRate;
    double frameRateInterval;
    int frames;
    int score;
    int lives;
} OSD;

void initOSD(OSD* osd);

extern const int margin;
extern const int line_height;
extern const int col_width;
