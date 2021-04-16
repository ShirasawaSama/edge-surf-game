#pragma once
#include "nanovg.h"

int backgroundImage, surferImage, playerImage, boardImage, objectsSmallImage;

void loadResources(NVGcontext* ctx);
void freeResources(NVGcontext* ctx);
