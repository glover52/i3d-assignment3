#pragma once

typedef struct {
    float time;
    float value;
} Keyframe;

typedef struct {
    float duration;
    float time;

    float value;

    size_t frame_count;

    Keyframe* keyframes;
} Interpolator;

float interpolate(Interpolator);

