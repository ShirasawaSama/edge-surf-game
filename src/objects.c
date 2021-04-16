#include "game.h"
#include "objects.h"
#include "utils.h"
#include <stdlib.h>

Object* makeSmallObject(float x, float y) {
	Object* obj = (Object*)malloc(sizeof(Object));
	obj->type = SMALL_OBJECT;
	obj->index = (int) (randomFloat() * 8);
	obj->x = x;
	obj->y = y;
	return obj;
}
