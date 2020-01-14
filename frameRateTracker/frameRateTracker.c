#include "frameRateTracker.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct frameRateTrackerInternal
{
    frameRateTracker frameRateTrackerExternal;

    uint32_t * window;
    uint32_t windowSizeMs;
    uint32_t numBoxes;

    uint32_t boxWidthMs;
    uint32_t lastReceivedWindowIndex;
    uint32_t lastReceivedIndex;

    uint32_t frameRate;
} frameRateTrackerInternal;


frameRateTracker * createFrameRateTracker(
        uint32_t windowSizeMs,
        uint32_t numBoxes
        )
{
    frameRateTrackerInternal * frt = malloc(sizeof(*frt));

    if (!frt)
    {
        fprintf(stderr, "Error! Couldn't allocate memory for frame-rate tracker\n");
        return NULL;
    }

    frt->lastReceivedWindowIndex = 0;
    frt->lastReceivedIndex = 0;
    frt->windowSizeMs = windowSizeMs;
    frt->numBoxes = numBoxes;
    frt->boxWidthMs = frt->windowSizeMs / frt->numBoxes;
    frt->frameRate = 0;

    frt->window = malloc(sizeof(*(frt->window)) * frt->numBoxes);

    if (!frt->window)
    {
        fprintf(stderr, "Error! Couldn't allocate memory for frame-rate tracker's window\n");
        free(frt);
        return NULL;
    }

    memset(frt->window, 0, sizeof(*(frt->window)) * frt->numBoxes); 

    return &frt->frameRateTrackerExternal;
}


void destroyFrameRateTracker(frameRateTracker * frte)
{
    frameRateTrackerInternal * frt = (frameRateTrackerInternal *)frte;

    free(frt->window);

    free(frt);
}

uint32_t recordFramesGetFrameRate(frameRateTrackerInternal * frt, uint32_t frames, uint32_t time)
{
    uint32_t newWindowIndex = time / frt->windowSizeMs;
    uint32_t newIndex = (time % frt->windowSizeMs) / frt->boxWidthMs;

    bool clearWindow = (newWindowIndex - frt->lastReceivedWindowIndex > 1);
    clearWindow |= ((newWindowIndex > frt->lastReceivedWindowIndex) && (newIndex == frt->lastReceivedIndex));

    if (clearWindow)
    {
        memset(frt->window, 0, sizeof(*(frt->window)) * frt->numBoxes); 
        frt->lastReceivedWindowIndex = newWindowIndex;
        frt->lastReceivedIndex = newIndex;
        frt->window[frt->lastReceivedIndex] = frames;
        frt->frameRate = frames;
        return frt->frameRate;
    }

    if (newIndex == frt->lastReceivedIndex)
    {
        frt->window[newIndex] += frames;
        frt->frameRate += frames;
        return frt->frameRate;
    }

    frt->frameRate -= frt->window[newIndex];
    frt->window[newIndex] = frames;
    frt->frameRate += frames;
    int i = (frt->lastReceivedIndex + 1) % frt->numBoxes;
    for (;; i = (i+1) % frt->numBoxes)
    {
        if (i == newIndex)
            break;
        frt->frameRate -= frt->window[i];
        frt->window[i] = 0;
    }
    frt->lastReceivedIndex = newIndex;
    frt->lastReceivedWindowIndex = newWindowIndex;

    return frt->frameRate;
}

void frameRateTrackerRecordFrame(frameRateTracker * frte, uint32_t time)
{
    frameRateTrackerInternal * frt = (frameRateTrackerInternal *)frte;
    recordFramesGetFrameRate(frt, 1, time); 
}

uint32_t frameRateTrackerGetFrameRate(frameRateTracker * frte, uint32_t time)
{
    frameRateTrackerInternal * frt = (frameRateTrackerInternal *)frte;
    return recordFramesGetFrameRate(frt, 0, time); 
}
