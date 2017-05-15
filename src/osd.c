#include "osd.h"

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
