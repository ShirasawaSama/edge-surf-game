#pragma once

#include "nanovg.h"
#include <stdbool.h>

double randomDouble();
float randomFloat();
void drawImage(NVGcontext* vg, int image, float alpha, float sx, float sy, float sw, float sh, float x, float y);
void openUrl();
void getUserName(char* ch);
bool isCapsLockHold(int mods);
