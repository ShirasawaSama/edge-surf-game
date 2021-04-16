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
extern int backgroundImage, playerImage, boardImage, objectsSmallImage, objectsBigImage, interfaceImage, naughtySurferImage;
extern int objectHitBoxs[][2], objectTextures[10];

int surfer = 0, initialSpeed = 4;

bool stop = true, finished = false, unlimitedHearts = false, unlimitedPower = false, hasDog = true;
int surferAction = 0, heart = 3, power = 0, rush = 0, coinCount = 0;
float distance = 0, offset = 0, speed = 0, playerX, playerY;

int invincibleTimer = 0, fallTimer = 0, boardTimer = 0, flyingTimer = 0;

NaughtySurfer naughtySurfer = { 0, 2, 0, 0, false };
Queue *objects;

void resetGame() {
    queue_destroy(objects);
    queue_new(&objects);
    stop = true;
    finished = unlimitedHearts = unlimitedPower = hasDog = false;
    heart = 3;
    distance = offset = speed = playerX = playerY = power = surferAction = rush = invincibleTimer = fallTimer = boardTimer = flyingTimer = coinCount = 0;
}

static void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    NVG_NOTUSED(scancode);
    NVG_NOTUSED(mods);
    if (fallTimer) return;
    switch (action) {
    case GLFW_REPEAT:
        switch (key) {
        case GLFW_KEY_S:
        case GLFW_KEY_DOWN:
            if (!stop && !rush && (unlimitedPower || power > 0)) {
                if (!surferAction) surferAction = 3;
                rush = 1;
                if (!unlimitedPower) power--;
                speed = initialSpeed * 2;
            }
            break;
        case GLFW_KEY_H:
            unlimitedHearts = true;
            break;
        case GLFW_KEY_P:
            unlimitedPower = true;
        }
        break;
    case GLFW_PRESS:
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_SPACE:
            if (stop) {
                resetGame();
                break;
            }
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
}

void initGame(NVGcontext* ctx, GLFWwindow* window) {
    loadResources(ctx);
    glfwSetKeyCallback(window, key);
    queue_new(&objects);
}

void destoryGame(NVGcontext* ctx) {
    queue_destroy(objects);
    freeResources(ctx);
}

void drawSurferOrigin(NVGcontext* ctx, int type, int action, float alpha, float x, float y) {
    float sx = action * 64.0F;
    drawImage(ctx, boardImage, alpha, sx, boardTimer / 10 * 64, 64, 64, x, y);
    drawImage(ctx, playerImage, alpha, sx, type * 64.0F, 64, 64, x, y);
}

void drawSurfer(NVGcontext* ctx) {
    float left = (width - 64) / 2.0F, top = height * SURFER_TOP;
    playerX = left + 32;
    playerY = top + 32;
    float alpha = (invincibleTimer && !surferAction) || (invincibleTimer / 30) % 2 ? 0.6F : 1.0F;
    int action = flyingTimer ? 9 + (flyingTimer / 8 % 4) : surferAction;
    drawSurferOrigin(ctx, surfer, action, alpha, left, top);
    if (hasDog) drawImage(ctx, playerImage, 1, action * 64.0F, 512, 64, 64, left, top);
    if (flyingTimer) {
        nvgFillColor(ctx, nvgRGBA(0, 0, 0, flyingTimer > 50 ? 50 : flyingTimer));
        nvgBeginPath(ctx);
        nvgEllipse(ctx, playerX, top + (flyingTimer > 50 ? 80.0F : 70.0F + flyingTimer / 5.0F), 16, 6);
        nvgFill(ctx);
    }
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

    if (unlimitedHearts) {
        drawImage(ctx, interfaceImage, 1, 0, 49, 24, 24, center - 105, 3);
        drawImage(ctx, interfaceImage, 1, 24, 0, 24, 24, center - 130, 3);
    } else {
        float left = center - 80;
        for (int i = 3 - heart; i-- > 0;) drawImage(ctx, interfaceImage, 1, 0, 0, 24, 24, left -= 25, 3);
        for (int i = heart; i-- > 0;) drawImage(ctx, interfaceImage, 1, 24, 0, 24, 24, left -= 25, 3);
    }
    if (unlimitedPower) {
        drawImage(ctx, interfaceImage, 1, 24, 24, 24, 24, center + 75, 3);
        drawImage(ctx, interfaceImage, 1, 0, 49, 24, 24, center + 100, 3);
    } else {
        float left = center + 50;
        for (int i = power; i-- > 0;) drawImage(ctx, interfaceImage, 1, 24, 24, 24, 24, left += 25, 3);
        for (int i = 3 - power; i-- > 0;) drawImage(ctx, interfaceImage, 1, 0, 24, 24, 24, left += 25, 3);
    }
    for (int i = coinCount, left = 10; i-- > 0; left += 24) drawImage(ctx, interfaceImage, 1, 24, 72, 24, 24, left, 5);
}

bool isNaughtySurferExists() {
    return naughtySurfer.visible && naughtySurfer.action != 2;
}

void hitPlayer() {
    if (fallTimer || invincibleTimer) return;
    speed = 0;
    stop = true;
    if (hasDog) {
        hasDog = false;
        float x = playerX + offset - 32, y = playerY + distance - 32;
        Object *obj = makeInteractObject(x, y);
        obj->index = 7;
        queue_enqueue(objects, obj);
        queue_enqueue(objects, makeRippleObject(x - 16, y - 25));
    } else if (!unlimitedHearts && --heart < 0) heart = 0;
    surferAction = 6;
    if (heart < 1) finished = true;
    else fallTimer = 1;
}

void drawObjects(NVGcontext* ctx) {
    if (distance > height && !fallTimer && !invincibleTimer) {
        int dis = (int)distance;
        if (dis % 103 <= speed && randomFloat() > 0.6) queue_enqueue(objects, makeSmallObject(randomFloat() * width + offset, distance + height));
        if (dis % 133 <= speed && randomFloat() > 0.5) queue_enqueue(objects, makeSlowdownObject(randomFloat() * width + offset, distance + height));
        if (dis % 133 <= speed && randomFloat() > 0.8) queue_enqueue(objects, makeAmbientObject(randomFloat() * width + offset, distance + height));
        if (dis % 1000 <= speed && randomFloat() > 0.996) queue_enqueue(objects, makeIslandObject(randomFloat() * width + offset, distance + height));
        if (dis % 103 <= speed && randomFloat() > 0.6) {
            double x = randomFloat() * width + offset, y = distance + height;
            queue_enqueue(objects, makeInteractObject(x, y));
            queue_enqueue(objects, makeRippleObject(x - 16, y - 18));
        }
        if (dis % 100 <= speed && randomFloat() > 0.4) {
            double x = randomFloat() * width + offset, y = distance + height;
            queue_enqueue(objects, makeBigObject(x, y));
            queue_enqueue(objects, makeRippleObject(x - 16, y - 18));
        }
        if (dis % 400 <= speed && randomFloat() > 0.7) {
            double x = randomFloat() * width + offset, y = distance + height;
            queue_enqueue(objects, makeSandBarObject(x, y));
        }
    }
    QueueIter iter;
    queue_iter_init(&iter, objects);
    Object *obj = NULL;
    while (queue_iter_next(&iter, &obj) != CC_ITER_END) {
        int type = obj->type;
        if (type < 0 || type > 5 && obj->y + objectHitBoxs[type][1] < distance) {
            queue_poll(objects, NULL);
            if (obj->index < 0) continue;
            free(obj);
            obj = NULL;
        } else {
            int boxX = objectHitBoxs[type][0], boxY = objectHitBoxs[type][1], maxX = boxX / 2, maxY = boxY / 2;
            float tx = obj->x - offset, ty = obj->y - distance;
            float x = maxX - 4, y = maxY * 0.7, cx = tx + maxX, cy = ty + maxY;
            if (!obj->once || obj->stage > 80) drawImage(ctx, objectTextures[type], 1, boxX * obj->index,
                boxY * ((obj->stage - (obj->once ? 80 : 0)) / 14), boxX, boxY, tx, ty);
            if (obj->maxStage) {
                if (obj->once) {
                    if (obj->stage <= obj->maxStage * 14 + 80) obj->stage++;
                } else if (++obj->stage > obj->maxStage * 14) obj->stage = 0;
            }
            if (!fallTimer && !invincibleTimer && !flyingTimer && playerX > cx - x && playerX < cx + x && playerY > cy - y && playerY < cy + y) switch (type) {
            case INTERACT_OBJECT:
                switch (obj->index) {
                case 0:
                    flyingTimer = 300;
                    goto out;
                case 2:
                    if (power < 5) power++;
                    break;
                case 3:
                    if (heart < 5) heart++;
                    break;
                case 4:
                    if (heart < 5) coinCount++;
                    break;
                case 6:
                    hasDog = true;
                    break;
                }
                obj->y = 0;
                out: break;
            case SMALL_OBJECT:
                surferAction = 1 + randomFloat() * 5;
                if (surferAction == 6) surferAction = 5;
                break;
            case SLOWDOWN_OBJECT: speed = 1;
            case RIPPLE_OBJECT: break;
            default: hitPlayer();
            }
            if (isNaughtySurferExists()) {
                float nx = naughtySurfer.x - offset + 32, ny = naughtySurfer.y - distance + 50;
                if (nx > cx - x && nx < cx + x && ny > cy - y && ny < cy + y) switch (type) {
                case SLOWDOWN_OBJECT:
                case INTERACT_OBJECT:
                case RIPPLE_OBJECT: break;
                default:
                    naughtySurfer.action = 2;
                    return;
                }
                if (!fallTimer && !invincibleTimer && !flyingTimer && playerX > nx - 32 && playerX < nx + 32 && playerY > ny - 50 && playerY < ny + 20) {
                    naughtySurfer.action = 2;
                    hitPlayer();
                }
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

void drawNaughtySurfer(NVGcontext* ctx) {
    if (randomFloat() > 0.997) {
        if (!naughtySurfer.visible) {
            naughtySurfer.y = distance + 5;
            naughtySurfer.x = width * randomFloat() + offset;
            naughtySurfer.type = randomFloat() * 9;
            naughtySurfer.action = randomFloat() * 2;
            naughtySurfer.visible = true;
        }
    }
    if (!naughtySurfer.visible) return;
    float dis = naughtySurfer.y - distance;
    if (dis < 0 || dis > height) {
        naughtySurfer.visible = false;
        return;
    }
    int sx = 64 * (naughtySurfer.type * 3 + naughtySurfer.action);
    if (naughtySurfer.action != 2) {
        if (randomFloat() > 0.97) naughtySurfer.action = naughtySurfer.action == 0;
        naughtySurfer.y += 6;
        naughtySurfer.x += naughtySurfer.action == 0 ? -6 : 6;
    }
    drawImage(ctx, naughtySurferImage, 1, sx, 65, 64, 64, naughtySurfer.x - offset, dis);
    drawImage(ctx, naughtySurferImage, 1, sx, 0, 64, 64, naughtySurfer.x - offset, dis);
}

void calcOffset() {
    if (stop) return;
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
    if (++boardTimer == 30) {
        srand((unsigned int)time(0));
        boardTimer = 0;
    }
    if (!finished) {
        calcOffset();
        if (fallTimer && fallTimer++ > 200) {
            fallTimer = 0;
            surferAction = 0;
            stop = true;
            speed = 0;
            invincibleTimer = 1;
        }
        if (invincibleTimer && surferAction != 0) {
            if (invincibleTimer++ > 300) invincibleTimer = 0;
        }
        if (flyingTimer > 0) flyingTimer--;
    }
    drawImage(ctx, backgroundImage, 1, (int)offset % 256, (int)distance % 256, width, height, 0, 0); // Background

    drawObjects(ctx);
    drawSurfer(ctx);
    drawNaughtySurfer(ctx);
    if (finished) drawFinishViewer(ctx);
    drawStatusBar(ctx);
}
