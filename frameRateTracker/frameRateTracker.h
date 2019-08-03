#ifndef FRAME_RATE_TRACKER_H
#define FRAME_RATE_TRACKER_H
#include <stdint.h>

typedef struct frameRateTracker
{
} frameRateTracker;

frameRateTracker * createFrameRateTracker(
        uint32_t windowSizeMs,
        uint32_t numBoxes
        );

void destroyFrameRateTracker(frameRateTracker * frt);

void frameRateTrackerRecordFrame(frameRateTracker * frt, uint32_t time);
uint32_t frameRateTrackerGetFrameRate(frameRateTracker * frt, uint32_t time);

#endif
