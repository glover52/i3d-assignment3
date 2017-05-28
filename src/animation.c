#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "animation.h"

static float lerp(Keyframe* k0, Keyframe* k1, float t) {
    return k0->value + (t - k0->time) * (k1->value - k0->value) / (k1->time - k0->time);
}

Interpolator* createInterpolator(size_t frame_count, float duration) {
    Interpolator* interpolator = malloc(sizeof(Interpolator));
    interpolator->duration = duration;
    interpolator->time = 0.0;
    interpolator->value = 0.0;
    interpolator->frame_count = frame_count;
    interpolator->keyframes = calloc(sizeof(Keyframe), frame_count);
    interpolator->attribute_to_update = NULL;
    return interpolator;
}

void interpolate(Interpolator* interpolator, float dt) {
    if (interpolator == NULL || interpolator->attribute_to_update == NULL) {
        return;
    }

    if (interpolator->time >= interpolator->duration) {
        interpolator->time = 0.0;
        return;
    }

    interpolator->time += dt;

    for (size_t i = 0; i < interpolator->frame_count; i++) {
        Keyframe* currentFrame = interpolator->keyframes + i;
        if (currentFrame->time >= interpolator->time) {
            if (i > 0) {
                Keyframe* previousFrame = currentFrame - 1;
                float newValue = lerp(previousFrame, currentFrame, interpolator->time);
                // printf("lerp (%.2f, %.2f) -> (%.2f, %.2f) == %.2f \n",
                // previousFrame->time, previousFrame->value, currentFrame->time, currentFrame->value, newValue);
                *(interpolator->attribute_to_update) = newValue;
            } else {
                *(interpolator->attribute_to_update) = currentFrame->value;
            }
            return;
        }
    }
}
