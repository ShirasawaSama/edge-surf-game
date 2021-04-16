#include "utils.h"
#include "game.h"
#include "resources.h"
#include "linkedList.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>

const float SURFER_TOP = 0.33F;

extern int width, height;
extern int backgroundImage, playerImage, boardImage, objectsSmallImage;

int surfer = 0, speed = 4;

int distance = 0, surferAction = 0;

enum ObjectType {
	SMALL_OBJECT
};

typedef struct Object {
	int type, index;
	float x, y;
} Object;

void initGame() {
	srand((unsigned int)time(0));
	//objects = listCreate(NULL);
}

void drawSurfer(NVGcontext* ctx) {
	float x = (surfer * 3 + surferAction) * 64.0F, top = height * SURFER_TOP, left = (width - 64) / 2.0F;
	drawImage(ctx, boardImage, 1, x, 0, 64, 64, left, top);
	drawImage(ctx, playerImage, 1, x, 0, 64, 64, left, top);
}

void drawObjects(NVGcontext* ctx) {
	/*while(objects != NULL) {
		LinkedList objPtr = *objects;
		if (objPtr == NULL) break;
		Object* obj = (Object*)objPtr->data;
		if (obj == NULL || obj->y > distance) break;
		listPop(objects);
	}
	if (distance > height) {
		printf("%d", distance);
		if (distance % 500 <= speed) {
			Object obj = { SMALL_OBJECT, 2, randomFloat() * width, distance + height };
			listPush(objects, &obj);
		}
	}
	LinkedList *node = objects;
	while (node != NULL) {
		Object* obj = (Object*)(*node)->data;
		if (obj == NULL) break;
		printf("344");
		drawImage(ctx, objectsSmallImage, 1, 32 * obj->index, 0, 32, 32, obj->x, obj->y);
		node = (*node)->next;
	}*/
}

void draw(NVGcontext* ctx) {
	distance += speed;

	drawImage(ctx, backgroundImage, 1, 0, distance % 256, width, height, 0, 0); // Background

	drawSurfer(ctx);
	drawObjects(ctx);
}
