#pragma once

#include "nanovg.h"

double randomDouble();
float randomFloat();
void drawImage(NVGcontext* vg, int image, float alpha, float sx, float sy, float sw, float sh, float x, float y);
void openUrl();
