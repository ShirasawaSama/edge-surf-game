#pragma once
#include <stdbool.h>

enum ObjectType {
	SMALL_OBJECT,
	BIG_OBJECT,
	SLOWDOWN_OBJECT,
	RIPPLE_OBJECT,
	AMBIENT_OBJECT,
	SAND_BAR_OBJECT,
	ISLAND_OBJECT,
	INTERACT_OBJECT,
	EFFECT_OBJECT
};

int objectHitBoxs[][2];

typedef struct Object {
	int type, index, stage, maxStage;
	float x, y;
	bool once;
} Object;

typedef struct NaughtySurfer {
	int type, action;
	float x, y;
	bool visible;
} NaughtySurfer;

Object* makeSmallObject(float x, float y);
Object* makeBigObject(float x, float y);
Object* makeSlowdownObject(float x, float y);
Object* makeRippleObject(float x, float y);
Object* makeAmbientObject(float x, float y);
Object* makeSandBarObject(float x, float y);
Object* makeIslandObject(float x, float y);
Object* makeInteractObject(float x, float y);
Object* makeInteractObjectWithIndex(float x, float y, int index);
Object* makeEffectObject(float x, float y, int index);
