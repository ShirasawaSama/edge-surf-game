#pragma once

enum ObjectType {
	SMALL_OBJECT
};

typedef struct Object {
	int type, index;
	float x, y;
} Object;

Object* makeSmallObject(float x, float y);
