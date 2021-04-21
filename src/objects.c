#include "game.h"
#include "objects.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>

int objectHitBoxs[10][2] = { {32, 32}, {64, 64}, {64, 64}, {96, 96}, {64, 64}, {256, 128}, { 1280, 512 }, { 64, 64 }, { 128, 128 }, { 64, 64 } };

Object* makeSmallObjectWithIndex(float x, float y, int index) {
    Object* obj = (Object*)malloc(sizeof(Object));
    obj->type = SMALL_OBJECT;
    obj->index = index;
    obj->x = x;
    obj->y = y;
    obj->stage = obj->maxStage = 0;
    obj->once = false;
    return obj;
}

Object* makeSmallObject(float x, float y) { return makeSmallObjectWithIndex(x, y, (int) floor(randomDouble() * 8)); }

Object* makeBigObjectWithIndex(float x, float y, int index) {
    Object* obj = (Object*)malloc(sizeof(Object));
    obj->type = BIG_OBJECT;
    obj->index = index;
    obj->x = x;
    obj->y = y;
    obj->stage = obj->maxStage = 0;
    obj->once = false;
    return obj;
}

Object* makeBigObject(float x, float y) {
    int index = (int)floor(randomDouble() * 30);
    return makeBigObjectWithIndex(x, y, index >= 25 && index <= 27 ? (int)floor(randomDouble() * 25) : index);
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
    obj->index = (int)floor(randomDouble() * 9);
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
    obj->index = (int)floor(randomDouble() * 4);
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
    obj->index = (int)floor(randomDouble() * 4);
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

Object* makeInteractObjectWithIndex(float x, float y, int index) {
    Object* obj = (Object*)malloc(sizeof(Object));
    obj->type = INTERACT_OBJECT;
    obj->index = index;
    obj->x = x;
    obj->y = y;
    obj->stage = 0;
    obj->maxStage = 3;
    obj->once = false;
    return obj;
}

Object* makeInteractObject(float x, float y) {
    int index = (int)floor(randomDouble() * 6);
    Object* obj = makeInteractObjectWithIndex(x, y, index == 5 ? 6 : index);
    return obj;
}

Object* makeEffectObject(float x, float y, int index) {
    Object* obj = (Object*)malloc(sizeof(Object));
    obj->type = EFFECT_OBJECT;
    obj->index = index;
    obj->x = x;
    obj->y = y;
    obj->stage = 0;
    obj->maxStage = 2;
    obj->once = false;
    return obj;
}

void makeDockObject(CC_List* list, float x, float y, int index) {
    Object* obj = (Object*)malloc(sizeof(Object));
    obj->type = DOCK_OBJECT;
    obj->index = index;
    obj->x = x;
    obj->y = y;
    obj->stage = obj->maxStage = 0;
    obj->once = false;
    cc_list_add(list, obj);
    if (index < 7 && randomFloat() > 0.5F) cc_list_add(list, makeSmallObjectWithIndex(x + 13 + randomFloat() * 6.0F, y + randomFloat() * 10.0F, 8 + (int)floor(randomFloat() * 12)));
}

void makeDockOnTop(float x, float y, CC_List* list, bool hasTop) { if (hasTop || randomFloat() > 0.8F) makeDockObject(list, x, y - 64, 4); }

void makeDocksObject(float x, float y, CC_List* list, bool hasTop) {
    y += 64;
    if (randomFloat() > 0.7F) {
        float it = randomFloat();
        makeDockObject(list, x, y, it > 0.5F ? 3 : 7);
        if (hasTop || it > 0.5F) makeDockOnTop(x, y, list, hasTop);
    } else {
        float left = x;
        makeDockObject(list, x, y, 0);
        makeDockOnTop(x, y, list, hasTop);
        for (int i = (int)randomFloat() * 3; i-- > 0;) {
            left += 64;
            float it = randomFloat();
            makeDockObject(list, left, y, it > 0.75F ? 1 : it > 0.5F ? 7 : it > 0.25F ? 8 : 9);
            if (!hasTop && it > 0.75F) makeDockOnTop(x, y, list, false);
        }
        makeDockObject(list, left += 64, y, 2);
        if (!hasTop) makeDockOnTop(x, y, list, false);
    }
}

void makeStarterObject(float width, float height, CC_List* list) {
    float centerX = width / 2.0F, centerY = height / 2.0F + 64;

    cc_list_add(list, makeInteractObjectWithIndex(centerX - 64 - 20, centerY, 3));
    cc_list_add(list, makeInteractObjectWithIndex(centerX, centerY, 2));

    float x = centerX - 64 * 6, y = centerY + 128;

    for (int i = 10 + (int)randomFloat() * 7; i-- > 0;) cc_list_add(list, makeSlowdownObject(x + randomFloat() * 450 - 250, y + randomFloat() * 300 - 200));
    for (int i = 5 + (int)randomFloat() * 5; i-- > 0;) cc_list_add(list, makeBigObject(x + randomFloat() * 450 - 250, y + randomFloat() * 300 - 200));
    for (int i = 3 + (int)randomFloat() * 3; i-- > 0;) cc_list_add(list, makeSmallObject(x + randomFloat() * 450 - 250, y + randomFloat() * 300 - 200));

    makeDockOnTop(x, y, list, true);
    makeDockObject(list, x, y, 0);
    makeDockObject(list, x += 64, y, 1);
    makeDockObject(list, x += 64, y, 8);
    makeDockObject(list, x += 64, y, 2);

    x = centerX + 96;

    for (int i = 10 + (int)randomFloat() * 7; i-- > 0;) cc_list_add(list, makeSlowdownObject(x + randomFloat() * 450, y + randomFloat() * 300 - 200));
    for (int i = 5 + (int)randomFloat() * 5; i-- > 0;) cc_list_add(list, makeBigObject(x + randomFloat() * 450, y + randomFloat() * 300 - 200));
    for (int i = 3 + (int)randomFloat() * 3; i-- > 0;) cc_list_add(list, makeSmallObject(x + randomFloat() * 450, y + randomFloat() * 300 - 200));

    makeDockObject(list, x, y, 0);
    makeDockObject(list, x += 64, y, 9);
    makeDockObject(list, x += 64, y, 1);
    makeDockObject(list, x += 64, y, 2);
    makeDockOnTop(x, y, list, true);

    x = centerX - 64 * 8.5F;
    y = centerY;
    makeDockOnTop(x, y, list, true);
    makeDockObject(list, x, y, 0);
    makeDockObject(list, x += 64, y, 9);
    makeDockObject(list, x += 64, y, 1);
    makeDockObject(list, x += 64, y, 1);
    makeDockObject(list, x += 64, y, 2);

    x = centerX + 64 * 3;

    makeDockObject(list, x, y, 0);
    makeDockObject(list, x += 64, y, 1);
    makeDockObject(list, x += 64, y, 8);
    makeDockObject(list, x += 64, y, 1);
    makeDockObject(list, x += 64, y, 2);
    makeDockOnTop(x, y, list, true);
}
