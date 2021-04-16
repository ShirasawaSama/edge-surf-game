#pragma once
#include "nanovg.h"
#include <GLFW/glfw3.h>

void initGame(NVGcontext* ctx, GLFWwindow* window);
void destoryGame(NVGcontext* ctx);
void draw(NVGcontext* ctx);
