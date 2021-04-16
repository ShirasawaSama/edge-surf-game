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
extern int backgroundImage, playerImage, boardImage, objectsSmallImage, objectsBigImage, interfaceImage;
extern int objectHitBoxs[], objectTextures[10];

int surfer = 0, initialSpeed = 4;

bool paused = false, stop = true, finished = false;
int surferAction = 0, heart = 3, power = 2, rush = 0;
float distance = 0, offset = 0, speed = 0, playerX, playerY;

Queue *objects;

static void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (action == GLFW_REPEAT && (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)) {
		if (!stop && !rush && power > 0) {
			if (!surferAction) surferAction = 3;
			rush = 1;
			power--;
			speed = initialSpeed * 2;
		}
		return;
	}
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
		if (stop) {
			speed = 1;
			stop = false;
		}
		surferAction = surferAction == 2 ? 1 : 2;
		break;
	case GLFW_KEY_RIGHT:
	case GLFW_KEY_D:
		if (stop) {
			speed = 1;
			stop = false;
		}
		surferAction = surferAction == 4 ? 5 : 4;
		break;
	case GLFW_KEY_UP:
	case GLFW_KEY_W:
		stop = true;
		speed = 0;
		surferAction = 0;
		rush = 0;
		break;
	case GLFW_KEY_DOWN:
	case GLFW_KEY_S:
		if (stop) {
			stop = false;
			speed = 1;
		}
		surferAction = 3;
	}
}

void initGame(NVGcontext* ctx, GLFWwindow* window) {
	srand((unsigned int)time(0));
	loadResources(ctx);
	glfwSetKeyCallback(window, key);
	queue_new(&objects);
}

void destoryGame(NVGcontext* ctx) {
	queue_destroy(objects);
	freeResources(ctx);
}

void drawSurfer(NVGcontext* ctx) {
	float x = (surfer * 3 + surferAction) * 64.0F, left = (width - 64) / 2.0F, top = height * SURFER_TOP;
	playerX = left + 32;
	playerY = top + 32;
	drawImage(ctx, boardImage, 1, x, 0, 64, 64, left, top);
	drawImage(ctx, playerImage, 1, x, 0, 64, 64, left, top);
}

void drawStatusBar(NVGcontext* ctx) {
	nvgBeginPath(ctx);
	nvgRect(ctx, 0, 0, width, 50);
	nvgPathWinding(ctx, NVG_HOLE);
	nvgFillPaint(ctx, nvgBoxGradient(ctx, -5, 0, width + 15, 45, 6, 10, nvgRGBA(0, 0, 0, 60), nvgRGBA(0, 0, 0, 0)));
	nvgFill(ctx);

	nvgFillColor(ctx, nvgRGB(255, 255, 255));
	nvgBeginPath(ctx);
	nvgRect(ctx, 0, 0, width, 40);
	nvgFill(ctx);

	float center = width / 2.0F;

	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
	nvgFontSize(ctx, 18);
	nvgFillColor(ctx, nvgRGBA(0, 0, 0, 230));
	char str[100];
	sprintf(str, "%d M", (int)(distance / 10.0));
	nvgText(ctx, center, 25, str, NULL);

	{
		float left = center - 80;
		for (int i = 0; i < heart; i++) drawImage(ctx, interfaceImage, 1, 25, 0, 24, 24, left -= 25, 3);
		for (int i = 0; i < 3 - heart; i++) drawImage(ctx, interfaceImage, 1, 0, 0, 24, 24, left -= 25, 3);
	}
	{
		float left = center + 50;
		for (int i = 0; i < power; i++) drawImage(ctx, interfaceImage, 1, 25, 25, 24, 24, left += 25, 3);
		for (int i = 0; i < 3 - power; i++) drawImage(ctx, interfaceImage, 1, 0, 25, 24, 24, left += 25, 3);
	}
}

void drawObjects(NVGcontext* ctx) {
	if (distance > height) {
		int dis = (int)distance;
		if (dis % 103 <= speed && randomFloat() > 0.7) queue_enqueue(objects, makeSmallObject(randomFloat() * width + offset, distance + height));
		if (dis % 200 <= speed && randomFloat() > 0.6) queue_enqueue(objects, makeBigObject(randomFloat() * width + offset, distance + height));
	}
	QueueIter iter;
	queue_iter_init(&iter, objects);
	Object *obj = NULL;
	while (queue_iter_next(&iter, &obj) != CC_ITER_END) {
		if (obj->y < distance) {
			queue_poll(objects, NULL);
			if (obj->index < 0) continue;
		 	free(obj);
			obj = NULL;
		} else {
			int type = obj->type, box = objectHitBoxs[type], max = box / 2;
			float tx = obj->x - offset, ty = obj->y - distance;
			float x = max - 4, y = max - 6, cx = tx + max, cy = ty + max;
			drawImage(ctx, objectTextures[type], 1, box * obj->index, 0, box, box, tx, ty);
			if (playerX > cx - x && playerX < cx + x && playerY > cy - y && playerY < cy + y) switch (type) {
			case SMALL_OBJECT:
				surferAction = 1 + randomFloat() * 5;
				if (surferAction == 6) surferAction = 5;
				break;
			default:
				speed = 0;
				surferAction = 6;
				finished = true;
			}
		}
	}
}

void drawFinishViewer(NVGcontext* ctx) {
	nvgBeginPath(ctx);
	nvgFillColor(ctx, nvgRGBA(255, 255, 255, 190));
	nvgRect(ctx, 0, 0, width, height);
	nvgFill(ctx);

	nvgFontSize(ctx, 50);
	nvgFillColor(ctx, nvgRGBA(0, 0, 0, 230));

	float centerX = width / 2.0F, centerY = height / 2.0F;
	nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
	nvgText(ctx, centerX, centerY - 42, "GAME OVER!", NULL);
	nvgFontSize(ctx, 22);
	nvgText(ctx, centerX, centerY, "Press the SPACE to start a new round.", NULL);
}

void calcOffset() {
	if (surferAction == 0) return;
	distance += speed;
	float ratio = 0;
	switch (surferAction) {
	case 1:
		ratio = -0.5;
		break;
	case 2:
		ratio = -0.2;
		break;
	case 5:
		ratio = 0.5;
		break;
	case 4: ratio = 0.2;
	}
	offset += ratio * speed;
	if (rush) if (rush >= 600) {
		rush = 0;
		speed = initialSpeed;
	} else rush++;
	if (speed < initialSpeed) speed *= 1.01;
}

void draw(NVGcontext* ctx) {
	if (!finished) calcOffset();
	drawImage(ctx, backgroundImage, 1, (int)offset % 256, (int)distance % 256, width, height, 0, 0); // Background

	drawSurfer(ctx);
	drawObjects(ctx);
	if (finished) drawFinishViewer(ctx);
	drawStatusBar(ctx);
}
