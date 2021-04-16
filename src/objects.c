#include "game.h"
#include "objects.h"
#include "utils.h"
#include <stdlib.h>

int objectHitBoxs[][2] = { {32, 32}, {64, 64}, {64, 64}, {96, 96}, {64, 64}, {256, 128}, { 1280, 512 }, { 64, 64} };

Object* makeSmallObject(float x, float y) {
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->type = SMALL_OBJECT;
	obj->index = (int) (randomFloat() * 8);
	obj->x = x;
	obj->y = y;
	obj->stage = obj->maxStage = 0;
	obj->once = false;
	return obj;
}

Object* makeBigObject(float x, float y) {
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->type = BIG_OBJECT;
	obj->index = (int)(randomFloat() * 30);
	obj->x = x;
	obj->y = y;
	obj->stage = obj->maxStage = 0;
	obj->once = false;
	return obj;
}

Object* makeRippleObject(float x, float y) {
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->type = RIPPLE_OBJECT;
	obj->x = x;
	obj->y = y;
	obj->stage = obj->index = 0;
	obj->maxStage = 2;
	obj->once = false;
	return obj;
}

Object* makeSlowdownObject(float x, float y) {
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->type = SLOWDOWN_OBJECT;
	obj->index = (int)(randomFloat() * 9);
	obj->x = x;
	obj->y = y;
	obj->stage = 0;
	obj->maxStage = 2;
	obj->once = false;
	return obj;
}

Object* makeAmbientObject(float x, float y) {
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->type = AMBIENT_OBJECT;
	obj->index = (int)(randomFloat() * 4);
	obj->x = x;
	obj->y = y;
	obj->stage = 0;
	obj->maxStage = 5;
	obj->once = true;
	return obj;
}

Object* makeSandBarObject(float x, float y) {
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->type = SAND_BAR_OBJECT;
	obj->index = (int)(randomFloat() * 4);
	obj->x = x;
	obj->y = y;
	obj->stage = obj->maxStage = 0;
	obj->once = false;
	return obj;
}

Object* makeIslandObject(float x, float y) {
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->type = ISLAND_OBJECT;
	obj->x = x;
	obj->y = y;
	obj->index = obj->stage = obj->maxStage = 0;
	obj->once = false;
	return obj;
}

Object* makeInteractObject(float x, float y) {
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->type = INTERACT_OBJECT;
	obj->index = (int)(randomFloat() * 6);
	if (obj->index == 5) obj->index = 6;
	obj->x = x;
	obj->y = y;
	obj->stage = 0;
	obj->maxStage = 3;
	obj->once = false;
	return obj;
}
