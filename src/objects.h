#pragma once

enum ObjectType {
	SMALL_OBJECT,
	BIG_OBJECT
};

int objectHitBoxs[];

typedef struct Object {
	int type, index;
	float x, y;
} Object;

Object* makeSmallObject(float x, float y);
Object* makeBigObject(float x, float y);
