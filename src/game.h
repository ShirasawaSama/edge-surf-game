#pragma once
#include "nanovg.h"
#include <GLFW/glfw3.h>

void initGame(GLFWwindow* window);
void destoryGame();
void draw(NVGcontext* ctx);
