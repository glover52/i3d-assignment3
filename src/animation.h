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

    float* attribute_to_update;
} Interpolator;

void interpolate(Interpolator*, float dt);
Interpolator* createInterpolator(size_t frame_count, float duration);
