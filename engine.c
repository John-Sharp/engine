#include "engine.h"

#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#include "listHeaders/actorList.h"
#include "listCode/actorList.inc"

#include "listHeaders/preLogicCallBackList.h"
#include "listCode/preLogicCallBackList.inc"

enum 
{
    MAX_TEXTURES = 50,
};

typedef struct engineInternal
{
    engine engineExternal;
    Uint32 startTime;

    bool shouldStartLogicLoop;
    unsigned int wholeFramesToDo;

    SDL_Window *window;
    SDL_Renderer *renderer;

    preLogicCallBackList * preLogicCallBackList;

    actorList * renderList;
    actorList * logicList;

    bool shouldContinue;

    juint numTextures;
    SDL_Texture * textures[MAX_TEXTURES];

} engineInternal;


actor *engineActorReg(engine * e, actor *a)
{
    engineInternal *eng = (engineInternal *)e;
    if (a->renderHandler)
    {
        eng->renderList = actorListAdd(eng->renderList, a);
    }

    if (a->logicHandler)
    {
        eng->logicList = actorListAdd(eng->logicList, a);
    }

    a->eng = e;

    return a;
}

engine *createEngine(
        unsigned int w,
        unsigned int h,
        void * owner)
{
    engineInternal * eng = malloc(sizeof(*eng));

    if (!eng)
    {
        fprintf(stderr, "Error! Couldn't allocate memory for engine\n");
        return NULL;
    }

    eng->engineExternal.fps = 80;
    eng->engineExternal.currentFrame = 0;
    eng->engineExternal.owner = owner;

    eng->startTime = 0;
    eng->shouldStartLogicLoop = true;
    eng->wholeFramesToDo = 0;

    SDL_Init(SDL_INIT_VIDEO);
    // TTF_Init();

    SDL_CreateWindowAndRenderer(w, h, 0, &eng->window, &eng->renderer);
    eng->engineExternal.w = w;
    eng->engineExternal.h = h;

    if(eng->window == NULL) {
        fprintf(
                stderr,
                "Window could not be created: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_SetRenderDrawColor(eng->renderer, 255, 255, 255, 255);

    SDL_StartTextInput();

    eng->preLogicCallBackList = NULL;
    eng->renderList = NULL;
    eng->logicList = NULL;

    eng->numTextures = 0;

    inputProcessorInit();

    return &eng->engineExternal;
}

void engineDestroy(engine * e)
{
    SDL_Quit();
    free(e);
}

bool shouldContinueLogicLoops(engineInternal * e)
{
    if (e->shouldStartLogicLoop) {
        unsigned int logicLoopStartTime = SDL_GetTicks();
        double elapsedFrames = (double)(logicLoopStartTime \
                - e->startTime) / 1000.0f * e->engineExternal.fps;

        e->wholeFramesToDo = (unsigned int)elapsedFrames - e->engineExternal.currentFrame;
    }

    if (!e->wholeFramesToDo) {
        e->shouldStartLogicLoop = true;
        return false;
    }

    e->wholeFramesToDo -= 1;
    e->engineExternal.currentFrame += 1;
    e->shouldStartLogicLoop = false;
    return true;
}

void processPreLogicCallBacks(engineInternal *e)
{
    preLogicCallBackList * pl;
    for (pl = e->preLogicCallBackList; pl != NULL; pl = pl->next)
    {
        ((preLogicCallBack)pl->val)(&e->engineExternal);
    }
}

void loopHandler(engineInternal *e)
{
    SDL_RenderClear(e->renderer);

    processInput();
    if (isStateActive(GS_QUIT))
    {
        e->shouldContinue = false;
        return;
    }

    actorList * al;
    for (al = e->renderList; al != NULL; al = al->next)
    {
        al->val->renderHandler(al->val);
    }

    while (shouldContinueLogicLoops(e))
    {
        processPreLogicCallBacks(e);

        for (al = e->logicList; al != NULL; al = al->next)
        {
            al->val->logicHandler(al->val);
        }
    }

    SDL_RenderPresent(e->renderer);
}

void engineStart(engine * e)
{
    engineInternal * engi = (engineInternal *)e;
    engi->startTime = SDL_GetTicks();

    // emscripten_set_main_loop(loop_handler, -1, 0);
    engi->shouldContinue = true;
    while (engi->shouldContinue)
    {
        loopHandler(engi);
    }
}

void engineReset(engine * e)
{
    //TODO
}

juint engineLoadTexture(engine *e, const char * fileName)
{
    engineInternal * eng = (engineInternal *)e;
    SDL_Surface *img = IMG_Load(fileName);

    if(!img){
        fprintf(stdout, "Error! Could not load %s\n", fileName);
        exit(1);
    }

    eng->textures[eng->numTextures++] = \
        SDL_CreateTextureFromSurface(eng->renderer, img);

    return eng->numTextures-1;
}

decal * decalInit(decal * d, engine * e, juint textureId, jintRect rect)
{
    engineInternal * eng = (engineInternal *)e;
    d->rect = rect;

    if (textureId >= eng->numTextures)
    {
        fprintf(stderr, "Warning: requested texture that did not exist\n");
        return NULL;
    }
    
    d->textureId = textureId;
    return d;
}

jintRect engineGetTextureRect(engine *e, juint textureId)
{
    engineInternal * eng = (engineInternal *)e;
    if (textureId >= eng->numTextures)
    {
        fprintf(stderr, "Warning: requested texture that did not exist\n");
        jintRect ret = {{0,0},{0,0}};
        return ret;
    }

    jintRect rect = {bl: {0, 0}};
    SDL_QueryTexture(eng->textures[textureId],
            NULL, NULL, &rect.tr[0], &rect.tr[1]);

    return rect;
}

void engineSpriteRender(engine * e, sprite *sp)
{
    SDL_Rect dest;
    engineInternal * eng = (engineInternal *)e;

    dest.x = sp->rect.bl[0];
    dest.y = e->h - sp->rect.tr[1];
    dest.w = sp->rect.tr[0] - sp->rect.bl[0];
    dest.h = sp->rect.tr[1] - sp->rect.bl[1];

    SDL_Rect src;
    jint texHeight;
    SDL_QueryTexture(eng->textures[sp->d->textureId],
            NULL, NULL, NULL, &texHeight);
    src.x = sp->d->rect.bl[0];
    src.y = texHeight - sp->d->rect.tr[1];
    src.w = sp->d->rect.tr[0] - sp->d->rect.bl[0];
    src.h = sp->d->rect.tr[1] - sp->d->rect.bl[1];

    SDL_RenderCopy (eng->renderer, eng->textures[sp->d->textureId],
            &src, &dest);
}

void enginePreLogicCallBackReg(engine * e, preLogicCallBack cb)
{
    engineInternal * eng = (engineInternal *)e;
    eng->preLogicCallBackList = preLogicCallBackListAdd(eng->preLogicCallBackList, (void *)cb);
}
