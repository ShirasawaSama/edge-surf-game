#pragma once
#include "nanovg.h"
#if defined DEBUG
#define ASSETS_ROOT "../assets/"
#else
#define ASSETS_ROOT "./assets/"
#endif
#define ASSETS_PATH(TO) ASSETS_ROOT TO
#define BGM_PATH ASSETS_PATH("bgm.mp3")

extern int backgroundImage, surferImage, playerImage, boardImage, interfaceImage, naughtySurferImage, enemyImage;
extern int objectTextures[];

void loadResources(NVGcontext* ctx);
void freeResources(NVGcontext* ctx);
