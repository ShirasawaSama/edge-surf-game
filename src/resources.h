#pragma once
#include "nanovg.h"

int backgroundImage, surferImage, playerImage, boardImage, objectsSmallImage, objectsBigImage, interfaceImage;
int objectTextures[];

void loadResources(NVGcontext* ctx);
void freeResources(NVGcontext* ctx);
