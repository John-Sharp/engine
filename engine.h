#ifndef ENGINE_H
#define ENGINE_H

// #include "../jTypes/jTypes.h"
#include "inputProcessor/inputProcessor.h"

typedef struct engine 
{
    unsigned int fps;

    unsigned int currentFrame;
    unsigned int w;
    unsigned int h;

    void * owner;
} engine;

typedef struct actor actor;
typedef void (*actorRenderHandler)(actor *a);
typedef void (*actorLogicHandler)(actor *a);

struct actor
{
    void * owner;
    actorRenderHandler renderHandler;
    actorLogicHandler logicHandler;
    engine * eng;
};

typedef struct decal decal;

typedef struct sprite
{
    decal *d;
    jintRect rect;
} sprite;

void engineSpriteRender(engine * e, sprite *sp);

struct decal
{
    juint textureId;
    jintRect rect;
};

decal * decalInit(decal * d, engine * e, juint textureId, jintRect rect);

engine * createEngine(
        unsigned int w,
        unsigned int h,
        void * owner);

void engineDestroy(engine * e);

void engineStart(engine * e);

void engineReset(engine * e);

juint engineLoadTexture(engine *e, const char * fileName);
juint engineCreateTexture(engine *e, Uint32 format, int access, int w, int h);
typedef void (*pixelUpdater)(void * pixels, int pitch, void * ctx);
void engineUpdateTexturesPixels(engine * e, juint texture, pixelUpdater pu, void * ctx);
jintRect engineGetTextureRect(engine * e, juint textureId);

actor *engineActorReg(engine * e, actor *a);
void actorEngineDereg(actor * a);

typedef void (*preLogicCallBack)(engine *e);
void enginePreLogicCallBackReg(engine * e, preLogicCallBack cb);

typedef void (*preRenderCallBack)(engine *e);
void enginePreRenderCallBackReg(engine * e, preRenderCallBack cb);

bool engineIsPaused(engine * e);
void enginePause(engine * e);
void engineUnpause(engine * e);

jintVec engineGetMouseLocation(engine * e);

void engineAdvanceFrames(engine * e, juint frames);

void engineGetFrameRate(engine * e, uint32_t * logicFrameRate, uint32_t * renderFrameRate);

#endif
