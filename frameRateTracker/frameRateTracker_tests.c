#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "frameRateTracker.h"

typedef struct ptrTableNode
{
    void * ptr;
    size_t allocated;
} ptrTableNode;

#include "listHeaders/ptrTableList.h"
#include "listCode/ptrTableList.inc"

typedef struct allocatorTracker
{
    int bytesAllocated;
    int bytesFreed;
    ptrTableList * ptrTableList;
} allocatorTrackerStruct;
allocatorTrackerStruct allocatorTracker;
void allocatorTracker_reset()
{
    allocatorTracker.bytesAllocated = 0;
    allocatorTracker.bytesFreed = 0;

    // TODO free ptrTableList
}

bool ptrTableNodePtrCmp(const ptrTableNode * a, const ptrTableNode * b)
{
    return (a->ptr == b->ptr);
}

bool (*mallocShouldFail)() = NULL;
void * myMalloc(size_t x)
{
    if (mallocShouldFail && mallocShouldFail())
    {
        return NULL;
    }

    void * p = malloc(x);

    allocatorTracker.bytesAllocated += x;
    ptrTableNode * node = malloc(sizeof(node));;
    node->ptr = p;
    node->allocated = x;

    allocatorTracker.ptrTableList = ptrTableListAdd(
        allocatorTracker.ptrTableList,
        node);

    return p;
}

void myFree(void * ptr)
{
    ptrTableNode node = {ptr, 0};
    ptrTableNode * foundNode;
    allocatorTracker.ptrTableList =  ptrTableListRm(
            allocatorTracker.ptrTableList,
            &node,
            ptrTableNodePtrCmp,
            &foundNode);

    allocatorTracker.bytesFreed += foundNode->allocated;
    free(foundNode);

    free(ptr);
}


bool mallocShouldFailOnFirstAttempt()
{
    return true;
}

bool mallocShouldFailOnSecondAttempt()
{
    static int calls = 0;
    calls++;
    if (calls == 2)
    {
        calls = 0;
        return true;
    }
    return false;
}

// create frameRateTracker, then destroy, check memory allocated = memory freed
// create frameRateTracker with failures in first and second calls to malloc, check
// failed attempts are properly cleaned up
void createFrameRateTrackerTest()
{
    allocatorTracker_reset();
    frameRateTracker * frt = createFrameRateTracker(1, 1);
    destroyFrameRateTracker(frt);

    if (allocatorTracker.bytesAllocated == 0)
    {
        printf("createFrameRateTrackerTest FAILED: no memory allocated for creation of frameRateTracker\n");
        exit(1);
    }

    if (allocatorTracker.bytesAllocated != allocatorTracker.bytesFreed)
    {
        printf("createFrameRateTrackerTest FAILED: memory freed upon destruction of frameRateTracker (%u)"
                " != memory allocated upon creation of frameRateTracker (%u)\n",
                allocatorTracker.bytesFreed,
                allocatorTracker.bytesAllocated);
        exit(1);
    }

    allocatorTracker_reset();

    mallocShouldFail = mallocShouldFailOnFirstAttempt; 
    frt = createFrameRateTracker(1, 1);

    if (frt != NULL)
    {
        printf("createFrameRateTrackerTest FAILED: when malloc fails createFrameRateTracker returns a non-NULL pointer\n");
        exit(1);
    }

    if (allocatorTracker.bytesAllocated != allocatorTracker.bytesFreed)
    {
        printf("createFrameRateTrackerTest FAILED: when malloc fails createFrameRateTracker still allocates memory\n");
        exit(1);
    }

    allocatorTracker_reset();

    mallocShouldFail = mallocShouldFailOnSecondAttempt; 

    frt = createFrameRateTracker(1, 1);

    if (frt != NULL)
    {
        printf("createFrameRateTrackerTest FAILED: when malloc fails on second call createFrameRateTracker returns a non-NULL pointer\n");
        exit(1);
    }

    if (allocatorTracker.bytesAllocated != allocatorTracker.bytesFreed)
    {
        printf("createFrameRateTrackerTest FAILED: when malloc fails on second call createFrameRateTracker still allocates memory\n");
        exit(1);
    }

    mallocShouldFail = NULL;
    allocatorTracker_reset();
}

// get frame rate initially at time 0
// check frame rate is calculated for frames at t = 0, t = winSize / 2, t =
// winSize/2, t = winSize-1 is 1, 2, 3, and 4, respectively
typedef struct frameRateTest
{
    uint32_t t;
    uint32_t fr;
} frameRateTest;

void initialWindowTest()
{
    uint32_t windowSizeMs = 1000;
    uint32_t numBoxes = 10;

    frameRateTracker * frt = createFrameRateTracker(
            windowSizeMs, numBoxes);

    frameRateTest frTests[] = {
        {.t = 0, .fr = 0},
        {.t = windowSizeMs / 2, .fr = 0},
        {.t = windowSizeMs -1, .fr = 0}
    };
    int i;
    for (i = 0; i < sizeof(frTests) / sizeof(frTests[0]); i++)
    {
        uint32_t time = frTests[i].t;
        uint32_t expected = frTests[i].fr;

        uint32_t frameRate = frameRateTrackerGetFrameRate(frt, time);
        if (frameRate != expected)
        {
            printf("initialWindowTest FAILED: frame rate != 0 in initial window "
                    "when no frames registered (at t = %u)\n", time);
            exit(1);
        }
    }

    uint32_t frameTimes[] = {0, windowSizeMs / 2, windowSizeMs /2, windowSizeMs - 1}; 
    frameRateTest frTestsB[] = {
        {.t = 0, .fr = 1},
        {.t = windowSizeMs / 2, .fr = 2},
        {.t = windowSizeMs / 2, .fr = 3},
        {.t = windowSizeMs -1, .fr = 4}
    };
    for (i = 0; i < sizeof(frameTimes) / sizeof(frameTimes[0]); i++)
    {
        frameRateTrackerRecordFrame(frt, frameTimes[i]);


        uint32_t time = frTestsB[i].t;
        uint32_t expected = frTestsB[i].fr;
        uint32_t frameRate = frameRateTrackerGetFrameRate(frt, time);
        if (frameRate != expected)
        {
            printf("initialWindowTest FAILED: frame rate != %u in initial window "
                    "(at t = %u), get %u\n", expected, time, frameRate);
            exit(1);
        }
    }

    destroyFrameRateTracker(frt);
}

// create frame at times t = 0, winSize / 2, winsize
// test that after time t + winSize after each frame creation the frame rate
// is 0
void outsideInitialWindowTest()
{
    uint32_t windowSizeMs = 1000;
    uint32_t numBoxes = 10;

    uint32_t times[] = {0, windowSizeMs/2 , windowSizeMs};
    int i;
    for (i = 0; i < sizeof(times) / sizeof(times[0]); i++)
    {
        frameRateTracker * frt = createFrameRateTracker(
                windowSizeMs, numBoxes);

        frameRateTrackerRecordFrame(frt, times[i]);
        uint32_t frameRate = frameRateTrackerGetFrameRate(frt, times[i] + windowSizeMs);
        if (frameRate != 0)
        {
            printf("outsideInitialWindowTest FAILED: frame rate != 0 outside initial window "
                    "(at t = %u), get %u\n", times[i] + windowSizeMs, frameRate);
            exit(1);
        }

        destroyFrameRateTracker(frt);
    }

}

// test that window moves correctly
void windowBorderTest()
{
    uint32_t windowSizeMs = 1000;
    uint32_t numBoxes = 10;

    frameRateTracker * frt = createFrameRateTracker(
            windowSizeMs, numBoxes);

    frameRateTrackerRecordFrame(frt, 0);
    frameRateTrackerRecordFrame(frt, 100);

    uint32_t frameRate = frameRateTrackerGetFrameRate(frt, windowSizeMs-1);
    if (frameRate != 2)
    {
        printf("windowBorderTest FAILED: frame rate != 2 when both frames should be in window "
                "(got frame rate %u)\n", frameRate);
        exit(1);
    }

    frameRate = frameRateTrackerGetFrameRate(frt, windowSizeMs);
    if (frameRate != 1)
    {
        printf("windowBorderTest FAILED: frame rate != 1 when first frame should be out of window "
                "(got frame rate %u)\n", frameRate);
        exit(1);
    }

    frameRate = frameRateTrackerGetFrameRate(frt, windowSizeMs + 99);
    if (frameRate != 1)
    {
        printf("windowBorderTest FAILED: frame rate != 1 when first frame should be out of window and second frame just in"
                "(got frame rate %u)\n", frameRate);
        exit(1);
    }

    frameRate = frameRateTrackerGetFrameRate(frt, windowSizeMs + 100);
    if (frameRate != 0)
    {
        printf("windowBorderTest FAILED: frame rate != 1 when second frame should be out of window "
                "(got frame rate %u)\n", frameRate);
        exit(1);
    }

    destroyFrameRateTracker(frt);
}

// tests that when the last received index is less than the new index then
// in-between values are correctly cleared
void lastReceivedIndexLTNewIndex()
{
    uint32_t windowSizeMs = 1000;
    uint32_t numBoxes = 10;

    frameRateTracker * frt = createFrameRateTracker(
            windowSizeMs, numBoxes);

    frameRateTrackerRecordFrame(frt, 3*100);
    frameRateTrackerRecordFrame(frt, 4*100);
    frameRateTrackerRecordFrame(frt, 5*100);
    frameRateTrackerRecordFrame(frt, 6*100);
    frameRateTrackerRecordFrame(frt, 12*100); // lastRecieved index will be 2

    uint32_t frameRate = frameRateTrackerGetFrameRate(frt, 12*100);

    if (frameRate != 5)
    {
        printf("lastReceivedIndexLTNewIndex FAILED: in preamble "
                ", get %u\n", frameRate);
        exit(1);
    }

    frameRateTrackerRecordFrame(frt, 17*100); // new index will be 7
    frameRate = frameRateTrackerGetFrameRate(frt, 17*100);

    if (frameRate != 2)
    {
        printf("lastReceivedIndexLTNewIndex FAILED: failed to clears frames between "
                "lastReceivedIndex and newIndex, get %u\n", frameRate);
        exit(1);
    }

    frameRateTrackerRecordFrame(frt, 35*100-1); // lastRecieved index will be 4
    frameRateTrackerRecordFrame(frt, 35*100); // new index will be 5

    if (frameRate != 2)
    {
        printf("lastReceivedIndexLTNewIndex FAILED: failed to clears frames when "
                "lastReceivedIndex = newIndex + 1, get %u\n", frameRate);
        exit(1);
    }
}

// tests that when the last received index is greater than the new index then
// in-between values are correctly cleared
void lastReceivedIndexGTNewIndex()
{
    uint32_t windowSizeMs = 1000;
    uint32_t numBoxes = 10;

    frameRateTracker * frt = createFrameRateTracker(
            windowSizeMs, numBoxes);

    frameRateTrackerRecordFrame(frt, 8*100);
    frameRateTrackerRecordFrame(frt, 9*100);
    frameRateTrackerRecordFrame(frt, 10*100);
    frameRateTrackerRecordFrame(frt, 11*100);
    frameRateTrackerRecordFrame(frt, 17*100); // last received index will be 7
    frameRateTrackerRecordFrame(frt, 22*100); // new index will be 2

    uint32_t frameRate = frameRateTrackerGetFrameRate(frt, 22*100);

    if (frameRate != 2)
    {
        printf("lastReceivedIndexGTNewIndex FAILED: failed to clear frames between "
                "lastReceivedIndex and newIndex, get %u\n", frameRate);
        exit(1);
    }

    frameRateTrackerRecordFrame(frt, 35*100); // lastRecieved index will be 5
    frameRateTrackerRecordFrame(frt, 45*100-1); // new index will be 4

    if (frameRate != 2)
    {
        printf("lastReceivedIndexGTNewIndex FAILED: failed to clears frames when "
                "lastReceivedIndex = newIndex - 1, get %u\n", frameRate);
        exit(1);
    }
}

// lastReceivedIndex last element in window, newIndex first
void borderStraddle()
{
    uint32_t windowSizeMs = 1000;
    uint32_t numBoxes = 10;

    frameRateTracker * frt = createFrameRateTracker(
            windowSizeMs, numBoxes);

    frameRateTrackerRecordFrame(frt, 9*100);
    frameRateTrackerRecordFrame(frt, 10*100);

    uint32_t frameRate = frameRateTrackerGetFrameRate(frt, 10*100);

    if (frameRate != 2)
    {
        printf("borderStraddle FAILED: failed to add frames "
                "when they straddle border, get %u\n", frameRate);
        exit(1);
    }
}

int main()
{
    createFrameRateTrackerTest();
    initialWindowTest();
    outsideInitialWindowTest();
    windowBorderTest();
    lastReceivedIndexLTNewIndex();
    lastReceivedIndexGTNewIndex();
    borderStraddle();

    return 0;
}

