#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "nanovg.h"
#include "nanovg_gl.h"
#include "main.h"
#include "resources.h"
#include "game.h"

int width, height;

int main() {
	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(1000, 600, "NanoVG", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
	glGetError();
	NVGcontext* ctx = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
	if (ctx == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}
	loadResources(ctx);
	initGame(window);
	printf("Loaded!\n");
	while (!glfwWindowShouldClose(window)) {
		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(56.0F / 255, 194.0F / 255, 238.0F / 255, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		nvgBeginFrame(ctx, width, height, (float) width / (float) height);
		draw(ctx);
		nvgEndFrame(ctx);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	destoryGame();
	freeResources(ctx);
	nvgDeleteGL3(ctx);
	glfwTerminate();
	return 0;
}
