#pragma once
#include "nanovg.h"

int backgroundImage, surferImage, playerImage, boardImage, interfaceImage, naughtySurferImage;
int objectTextures[];

void loadResources(NVGcontext* ctx);
void freeResources(NVGcontext* ctx);
