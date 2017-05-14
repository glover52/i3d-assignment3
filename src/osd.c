#include "osd.h"

void initOSD(OSD* osd) {
    osd->frameRate = 1.0;
    osd->frameRateInterval = 0.2;
    osd->frames = 0;
    osd->score = 0;
}

