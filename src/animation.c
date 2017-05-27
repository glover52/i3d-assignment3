#include <stdlib.h>
#include "animation.h"

static float lerp(Keyframe* k0, Keyframe* k1, float t) {
    return k0->value + (t - k0->time) * (k1->value - k0->value) / (k1->time - k0->time);
}

float interpolate(Interpolator interpolator) {
    if (interpolator.time >= interpolator.duration) {
        return 0.0f;
    }

    for (size_t i = 0; i < interpolator.frame_count; i++) {
        Keyframe* currentFrame = interpolator.keyframes + i;
        if (currentFrame->time >= interpolator.time) {
            if (i > 0) {
                Keyframe* previousFrame = currentFrame - 1;
                float newValue = lerp(previousFrame, currentFrame, interpolator.time);
                return newValue;
            }
            return currentFrame->value;
        }
    }

    return 0.0f;
}

