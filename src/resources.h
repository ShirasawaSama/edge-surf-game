#pragma once
#include "nanovg.h"

extern int backgroundImage, surferImage, playerImage, boardImage, interfaceImage, naughtySurferImage, enemyImage;
extern int objectTextures[];

void loadResources(NVGcontext* ctx);
void freeResources(NVGcontext* ctx);
