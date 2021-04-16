#include "utils.h"
#include "game.h"
#include "resources.h"
#include "objects.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include "cc_queue.h"

const float SURFER_TOP = 0.33F;

extern int width, height;
extern int backgroundImage, playerImage, boardImage, objectsSmallImage;

int surfer = 0, speed = 0;

bool paused = false, stop = false;
int distance = 0, surferAction = 0;
float offset = 0;

Queue *objects;

static void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (action != GLFW_PRESS) return;
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_SPACE:
		paused = !paused;
		break;
	case GLFW_KEY_LEFT:
	case GLFW_KEY_A:
		speed = 4;
		surferAction = surferAction == 2 ? 1 : 2;
		break;
	case GLFW_KEY_RIGHT:
	case GLFW_KEY_D:
		speed = 4;
		surferAction = surferAction == 4 ? 5 : 4;
		break;
	case GLFW_KEY_UP:
	case GLFW_KEY_W:
		stop = true;
		speed = 0;
		surferAction = 0;
		break;
	case GLFW_KEY_DOWN:
	case GLFW_KEY_S:
		stop = false;
		speed = 4;
		surferAction = 3;
	}
}

void initGame(GLFWwindow* window) {
	srand((unsigned int)time(0));
	glfwSetKeyCallback(window, key);
	queue_new(&objects);
}

void destoryGame() {
	queue_destroy(objects);
}

void drawSurfer(NVGcontext* ctx) {
	float x = (surfer * 3 + surferAction) * 64.0F, top = height * SURFER_TOP, left = (width - 64) / 2.0F;
	drawImage(ctx, boardImage, 1, x, 0, 64, 64, left, top);
	drawImage(ctx, playerImage, 1, x, 0, 64, 64, left, top);
}

void drawObjects(NVGcontext* ctx) {
	if (distance > height) {
		if (distance % 103 <= speed && randomFloat() > 0.7) {
			queue_enqueue(objects, makeSmallObject(randomFloat() * width + offset, distance + height));
		}
	}
	QueueIter iter;
	queue_iter_init(&iter, objects);
	Object *obj = NULL;
	while (queue_iter_next(&iter, &obj) != CC_ITER_END) {
		if (obj->y < distance) {
			queue_poll(objects, NULL);
			free(obj);
			obj = NULL;
		} else drawImage(ctx, objectsSmallImage, 1, 32 * obj->index, 0, 32, 32, obj->x - offset, obj->y - distance);
	}
}

void calcOffset() {
	if (surferAction == 0) return;
	distance += speed;
	float ratio = 0;
	switch (surferAction) {
	case 1:
		ratio = -0.4;
		break;
	case 2:
		ratio = -0.2;
		break;
	case 5:
		ratio = 0.4;
		break;
	case 4: ratio = 0.2;
	}
	offset += ratio * speed;
}

void draw(NVGcontext* ctx) {
	calcOffset();
	drawImage(ctx, backgroundImage, 1, (int)offset % 256, distance % 256, width, height, 0, 0); // Background

	drawSurfer(ctx);
	drawObjects(ctx);
}
