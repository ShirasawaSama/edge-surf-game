#ifdef _WIN32
#pragma warning(disable:4244)
#endif
#include "utils.h"
#include "game.h"
#include "resources.h"
#include "objects.h"
#include "cc_list.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

const float SURFER_TOP = 0.33F;
const int ANIMATION_TIMER_MAX_VALUE = 40, SETTING_WIDTH = 150, SETTING_TOP = 64;

extern bool playPaused, vSync;
extern int width, height, fps;
extern int backgroundImage, playerImage, boardImage, objectsSmallImage, objectsBigImage, interfaceImage, naughtySurferImage, enemyImage;
extern int objectHitBoxs[][2], objectTextures[10];

double cursorX = 0, cursorY = 0;

struct Score {
    int score;
    char username[30];
} scores[10];
char username[30];
int surfer = 0, initialSpeed = 4;

bool paused, finished, unlimitedHearts, unlimitedPower, hasDog, started, enemyStoped, editMode, settingOpen = false;
int surferAction, heart, power, coinCount, randomSeed;
float distance, offset, speed, playerX, playerY, enemyX, enemyY;

int invincibleTimer, fallTimer, boardTimer, flyingTimer, changeDirectionTimer, rushTimer, enemyTimer, boardBrokenTimer, animationTimer, cheatWarningTimer;

NaughtySurfer naughtySurfer = { 0, 2, 0, 0, false };
CC_List *objects = NULL;

bool isInvincible() { return fallTimer || invincibleTimer; }
bool isNotRecordScore() { return unlimitedHearts || unlimitedPower; }

bool isNaughtySurferExists() { return naughtySurfer.visible && naughtySurfer.action != 2; }
int getScore() { return (int)(distance / 10.0F) + hasDog * 1000 + coinCount * 2000 + power * 300; }

float randomX() { return randomFloat() * width + offset; }
float randomY() { return distance + height * (1 + randomFloat()); }

int comp(const void* a, const void* b) { return ((struct Score*)b)->score - ((struct Score*)a)->score; }

void addObject(Object* obj) { cc_list_add(objects, obj); }

void refreshScore() {
    FILE* fp = fopen("scores.txt", "a+");
    if (!fp) return;
    for (int i = 0; i < 10 && fscanf(fp, "%d %s", &scores[i].score, scores[i].username) != EOF; i++);
    fclose(fp);
    if (isNotRecordScore()) return;
    int score = getScore();
    if (score > scores[9].score) {
        scores[9].score = score;
        strcpy(scores[9].username, username);
        qsort(scores, 10, sizeof(struct Score), comp);
        char data[500] = { 0 }, tmp[100];
        for (int i = 0; i < 10; i++) {
            sprintf(tmp, "%d %s\n", scores[i].score, scores[i].username);
            strcat(data, tmp);
        }
        fp = fopen("scores.txt", "w");
        if (!fp) return;
        fputs(data, fp);
        fclose(fp);
    }
}

void resetGame() {
    if (objects != NULL) cc_list_destroy(objects);
    cc_list_new(&objects);
    paused = true;
    heart = 3;
    animationTimer = 1;
    srand((int)time(NULL));
    for (int i = rand() % 10; i-- > 0;) srand(rand());
    randomSeed = rand();
    finished = unlimitedHearts = unlimitedPower = hasDog = naughtySurfer.visible = started = enemyStoped = editMode = false;
    distance = offset = speed = playerX = playerY = enemyX = enemyY = power = surferAction = rushTimer = invincibleTimer = fallTimer = boardTimer =
        flyingTimer = changeDirectionTimer = coinCount = enemyTimer = boardBrokenTimer = cheatWarningTimer = 0;
}

void stopPlayer() {
    if (rushTimer) return;
    paused = true;
    speed = 0;
    surferAction = 0;
    rushTimer = 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!started) {
        if (action != GLFW_PRESS) return;
        if (editMode) {
            int len = (int) strlen(username);
            if (key == GLFW_KEY_BACKSPACE) {
                if (len) username[len - 1] = 0;
                return;
            }
            if (len >= 30) return;
            if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
                username[len] = key - (isCapsLockHold(mods) ? 0 : 'A' - 'a');
                return;
            } else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
                username[len] = key;
                return;
            }
        }
        switch (key) {
        case GLFW_KEY_SPACE:
            if (randomSeed) srand(randomSeed);
            makeStarterObject(width, height, objects);
            started = true;
            editMode = false;
            break;
        case GLFW_KEY_LEFT:
        case GLFW_KEY_A:
            if (surfer > 0) surfer--;
            break;
        case GLFW_KEY_RIGHT:
        case GLFW_KEY_D:
            if (surfer < 7) surfer++;
            break;
        case GLFW_KEY_BACKSPACE:
            if (randomSeed) randomSeed /= 10;
            break;
        default: {
            if (key < GLFW_KEY_0 || key > GLFW_KEY_9) break;
            int randomSeed1 = randomSeed * 10 + key - GLFW_KEY_0;
            if (randomSeed1 > 10000000) break;
            randomSeed = randomSeed1;
        } }
        return;
    }
    if (finished) {
        if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) resetGame();
        return;
    }
    if (fallTimer) return;
    switch (action) {
    case GLFW_REPEAT:
        switch (key) {
        case GLFW_KEY_S:
        case GLFW_KEY_DOWN:
            if (!paused && !rushTimer && (unlimitedPower || power > 0)) {
                if (!surferAction) surferAction = 3;
                rushTimer = 1;
                if (!unlimitedPower) power--;
                speed = initialSpeed * 2;
            }
            break;
        case GLFW_KEY_H:
            unlimitedHearts = true;
            if (!cheatWarningTimer) cheatWarningTimer = 1;
            break;
        case GLFW_KEY_P:
            unlimitedPower = true;
            if (!cheatWarningTimer) cheatWarningTimer = 1;
        }
        break;
    case GLFW_PRESS:
        switch (key) {
        case GLFW_KEY_SPACE:
            if (finished) resetGame();
            break;
        case GLFW_KEY_LEFT:
        case GLFW_KEY_A:
            if (paused) {
                speed = 1;
                paused = false;
            }
            surferAction = surferAction == 2 ? 1 : 2;
            break;
        case GLFW_KEY_RIGHT:
        case GLFW_KEY_D:
            if (paused) {
                speed = 1;
                paused = false;
            }
            surferAction = surferAction == 4 ? 5 : 4;
            break;
        case GLFW_KEY_UP:
        case GLFW_KEY_W:
            stopPlayer();
            break;
        case GLFW_KEY_DOWN:
        case GLFW_KEY_S:
            if (paused) {
                paused = false;
                speed = 1;
            }
            surferAction = 3;
        }
    }
    (void)scancode;
    (void)window;
}

void clickCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action != GLFW_PRESS || button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (cursorX >= width - 36 && cursorY <= 50) settingOpen = !settingOpen;
    if (!settingOpen || cursorX < width - SETTING_WIDTH - 16) {
        if (cursorY >= height - 90 && cursorY <= height - 70) editMode = !editMode;
        return;
    }
    if (cursorY > 64 && cursorY < 80) playPaused = !playPaused;
    else if (cursorY >= 80 && cursorY < 100) initialSpeed = initialSpeed == 4 ? 2 : 4;
    else if (cursorY >= 100 && cursorY < 130) glfwSwapInterval(vSync = !vSync);
    else if (cursorY >= 130 && cursorY < 150) { if (started && !finished) resetGame(); }
    else if (cursorY >= 150 && cursorY < 194) openUrl();
    (void)window;
    (void)mods;
}

void cursorCallback(GLFWwindow* window, double x, double y) {
    cursorX = x;
    cursorY = y;
    (void)window;
}

void initGame(NVGcontext* ctx, GLFWwindow* window) {
    loadResources(ctx);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, clickCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    getUserName(username);
    resetGame();
}

void destoryGame(NVGcontext* ctx) {
    cc_list_destroy(objects);
    freeResources(ctx);
}

void drawSurferOrigin(NVGcontext* ctx, int type, int action, float alpha, float x, float y) {
    float sx = action * 64.0F;
    drawImage(ctx, boardImage, alpha, sx, boardTimer % 30 / 10 * 64, 64, 64, x, y);
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

void drawSettings(NVGcontext* ctx) {
    nvgFillColor(ctx, nvgRGB(0, 0, 0));
    nvgBeginPath(ctx);
    nvgMoveTo(ctx, width - 36, 15);
    nvgLineTo(ctx, width - 16, 15);
    nvgMoveTo(ctx, width - 36, 23);
    nvgLineTo(ctx, width - 16, 23);
    nvgMoveTo(ctx, width - 36, 31);
    nvgLineTo(ctx, width - 16, 31);
    nvgStrokeWidth(ctx, 2);
    nvgFill(ctx);

    if (!settingOpen) return;

    float left = width - SETTING_WIDTH - 16, top = SETTING_TOP - 2;
    nvgFillColor(ctx, nvgRGBA(255, 255, 255, 220));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, left, SETTING_TOP, SETTING_WIDTH, 140, 2);
    nvgFill(ctx);

    left += 10;

    char str[100];
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFontSize(ctx, 16);
    
    nvgFillColor(ctx, playPaused ? nvgRGB(233, 30, 99) : nvgRGBA(0, 0, 0, 220));
    nvgText(ctx, left, top += 10, "Mute Music", NULL);
    nvgFillColor(ctx, initialSpeed == 2 ? nvgRGB(233, 30, 99) : nvgRGBA(0, 0, 0, 220));
    nvgText(ctx, left, top += 20, "Slowdown Mode", NULL);
    nvgFillColor(ctx, vSync ? nvgRGBA(0, 0, 0, 220) : nvgRGB(233, 30, 99));
    nvgText(ctx, left, top += 20, "Disable vSync", NULL);
    nvgFillColor(ctx, nvgRGBA(0, 0, 0, !started || finished ? 150 : 220));
    nvgText(ctx, left, top += 20, "Restart Game", NULL);

    nvgFontSize(ctx, 14);
    nvgFillColor(ctx, nvgRGBA(0, 0, 0, 170));
    sprintf(str, "FPS: %d", fps);
    nvgText(ctx, left, top += 20, str, NULL);

    sprintf(str, "Objects: %d", (int) cc_list_size(objects));
    nvgText(ctx, left, top += 16, str, NULL);
    nvgText(ctx, left, top += 16, "By: Shirasawa", NULL);
}

void drawStatusBar(NVGcontext* ctx) {
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 0, width, 60);
    nvgPathWinding(ctx, NVG_HOLE);
    nvgFillPaint(ctx, nvgBoxGradient(ctx, -5, 0, width + 15, 55, 6, 10, nvgRGBA(0, 0, 0, 60), nvgRGBA(0, 0, 0, 0)));
    nvgFill(ctx);

    nvgFillColor(ctx, nvgRGB(255, 255, 255));
    nvgBeginPath(ctx);
    nvgRect(ctx, 0, 0, width, 50);
    nvgFill(ctx);

    drawSettings(ctx);

    float center = width / 2.0F;

    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
    nvgFontSize(ctx, 18);
    nvgFillColor(ctx, nvgRGBA(0, 0, 0, 230));
    char str[100];
    sprintf(str, "%d M", (int)(distance / 10.0));
    nvgText(ctx, center, 35, str, NULL);

    if (unlimitedHearts) {
        drawImage(ctx, interfaceImage, 1, 0, 49, 24, 24, center - 105, 13);
        drawImage(ctx, interfaceImage, 1, 24, 0, 24, 24, center - 130, 13);
    } else {
        float left = center - 80;
        for (int i = 3 - heart; i-- > 0;) drawImage(ctx, interfaceImage, 1, 0, 0, 24, 24, left -= 25, 13);
        for (int i = heart; i-- > 0;) drawImage(ctx, interfaceImage, 1, 24, 0, 24, 24, left -= 25, 13);
    }
    if (unlimitedPower) {
        drawImage(ctx, interfaceImage, 1, 24, 24, 24, 24, center + 75, 13);
        drawImage(ctx, interfaceImage, 1, 0, 49, 24, 24, center + 100, 13);
    } else {
        float left = center + 56;
        for (int i = power; i-- > 0;) drawImage(ctx, interfaceImage, 1, 24, 24, 24, 24, left += 25, 13);
        for (int i = 3 - power; i-- > 0;) drawImage(ctx, interfaceImage, 1, 0, 24, 24, 24, left += 25, 13);
    }
    float left = 10;
    if (hasDog) {
        drawImage(ctx, interfaceImage, 1, 24, 48, 24, 24, left, 14);
        left += 25;
    }
    if (coinCount) {
        drawImage(ctx, interfaceImage, 1, 24, 72, 24, 24, left, 15);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);
        nvgFillColor(ctx, nvgRGBA(0, 0, 0, 230));
        sprintf(str, "x%d", coinCount);
        nvgText(ctx, left += 27, 37, str, NULL);
    }
}

void kickDog() {
    if (!hasDog) return;
    hasDog = false;
    float x = playerX + offset - 32, y = playerY + distance - 32;
    cc_list_add(objects, makeRippleObject(x - 16, y - 25));
    cc_list_add(objects, makeInteractObjectWithIndex(x, y, 7));
}

void hitPlayer() {
    if (isInvincible()) return;
    if (hasDog) {
        kickDog();
        invincibleTimer = 170;
        return;
    }
    speed = 0;
    paused = true;
    if (!unlimitedHearts && --heart < 0) heart = 0;
    surferAction = 6;
    if (heart < 1) {
        finished = true;
        animationTimer = 1;
        refreshScore();
    } else fallTimer = 1;
}

void generateObjects() {
    if (paused) return;
    int dis = (int)distance;
    bool isFar = dis > 10000;
    float chanceModify = ((int) (distance / 10000)) * 0.03F;
    if (chanceModify > 0.16F) chanceModify = 0.16F;
    if (dis % 73 <= speed && randomFloat() > 0.45F - chanceModify) addObject(makeSmallObject(randomX(), randomY()));
    if (dis % 44 <= speed && randomFloat() > 0.65F - chanceModify) addObject(makeSlowdownObject(randomX(), randomY()));
    if (dis % 133 <= speed && randomFloat() > 0.93F - chanceModify) addObject(makeAmbientObject(randomX(), randomY()));
    if (dis % 633 <= speed && randomFloat() > 0.5F - chanceModify) makeDocksObject(randomX(), randomY(), objects, false);
    if (isFar && dis % (height * 3) <= speed && randomFloat() > 0.9) { // Island
        float x = randomX(), y = randomY();
        addObject(makeIslandObject(x, y));
        addObject(makeInteractObjectWithIndex(x + 620, y + 94, 0));
        addObject(makeEffectObject(x + 800, y + 140, 5));
    }
    if (dis % 175 <= speed && randomFloat() > 0.7F) { // Interact
        double x = randomX(), y = randomY();
        addObject(makeRippleObject(x - 16, y - 18));
        addObject(makeInteractObject(x, y));
    }
    if (dis % 77 <= speed && randomFloat() > 0.36F - chanceModify) { // Big
        double x = randomX(), y = randomY();
        addObject(makeRippleObject(x - 16, y - 18));
        addObject(makeBigObject(x, y));
    }
    if (dis % 433 <= speed && randomFloat() > 0.7F - chanceModify) { // SandBar
        double x = randomX(), y = randomY();
        addObject(makeSandBarObject(x, y));
        if (randomFloat() > 0.7F) makeDocksObject(x + randomFloat() * 198, y + 128, objects, true);
    }
    if (isFar && dis % 589 <= speed && randomFloat() > 0.9F) {
        addObject(makeEffectObject(randomX(), randomY(), 1));
        addObject(makeEffectObject(randomX(), randomY(), 1));
    }
    if (isFar && dis % 512 <= speed && randomFloat() > 0.9F - chanceModify) {
        double x = randomX(), y = randomY();
        addObject(makeBigObjectWithIndex(x + 150, y + 150, 25));
        for (int i = 1 + randomFloat() * 3; i-- >= 0;) addObject(makeInteractObjectWithIndex(x + randomFloat() * 300, y + randomFloat() * 300, 1));
        for (int i = randomFloat() * 3; i-- >= 0;) addObject(makeSlowdownObject(x + randomFloat() * 300, randomY() + y * 300));
        for (int i = randomFloat() * 3; i-- >= 0;) addObject(makeBigObject(x + randomFloat() * 300, randomY() + y * 300));
        for (int i = randomFloat() * 3; i-- >= 0;) addObject(makeSmallObject(x + randomFloat() * 300, randomY() + y * 300));
    }
}

void drawObjects(NVGcontext* ctx) {
    CC_ListIter iter;
    cc_list_iter_init(&iter, objects);
    Object *obj = NULL;
    while (cc_list_iter_next(&iter, (void**)&obj) != CC_ITER_END) {
        int type = obj->type, boxX = objectHitBoxs[type][0], boxY = objectHitBoxs[type][1];
        if (obj->y + boxY < distance) {
            cc_list_iter_remove(&iter, NULL);
            free(obj);
            obj = NULL;
        } else {
            int maxX = boxX / 2, maxY = boxY / 2;
            float tx = obj->x - offset, ty = obj->y - distance;
            float x = maxX - 4, y = maxY * 0.7, cx = tx + maxX, cy = ty + maxY;
            switch (type) {
            case SAND_BAR_OBJECT:
                y = 20;
                break;
            case INTERACT_OBJECT:
                if (obj->index == 1) x = y = 70;
                break;
            case DOCK_OBJECT:
                x = y = 38;
                break;
            case ISLAND_OBJECT: y = 125;
            }
            if (!obj->once || obj->stage > 80) drawImage(ctx, objectTextures[type], 1, boxX * obj->index,
                boxY * ((obj->stage - (obj->once ? 80 : 0)) / 14), boxX, boxY, tx, ty);
            if (obj->maxStage) {
                if (obj->once) {
                    if (obj->stage <= obj->maxStage * 14 + 80) obj->stage++;
                } else if (++obj->stage > obj->maxStage * 14) obj->stage = 0;
            }
            if (finished) continue;
            if (!flyingTimer && playerX > cx - x && playerX < cx + x && playerY > cy - y && playerY < cy + y) switch (type) {
            case INTERACT_OBJECT:
                switch (obj->index) {
                case 0:
                    flyingTimer = 300;
                    if (enemyTimer) enemyStoped = true;
                case 7: goto out;
                case 1:
                    if (isInvincible()) goto out;
                    enemyTimer = 1;
                    enemyX = obj->x;
                    enemyY = obj->y;
                    break;
                case 2:
                    if (power < 5) power++;
                    break;
                case 3:
                    if (heart < 5) heart++;
                    break;
                case 4:
                    coinCount++;
                    break;
                case 6: hasDog = true;
                }
                cc_list_iter_remove(&iter, NULL);
                free(obj);
                obj = NULL;
                continue;
                out: break;
            case EFFECT_OBJECT:
                if (obj->index == 1) {
                    CC_ListIter iter2;
                    cc_list_iter_init(&iter2, objects);
                    Object* obj2 = NULL;
                    while (cc_list_iter_next(&iter2, (void**)&obj2) != CC_ITER_END) if (obj2->type == EFFECT_OBJECT && obj2 != obj && obj2->index == 1) {
                        obj->index = obj2->index = 0;
                        invincibleTimer = 150;
                        offset = obj2->x - playerX + 32;
                        distance = obj2->y - playerY + 32;
                        break;
                    }
                }
                break;
            case SMALL_OBJECT:
                if (isInvincible() || changeDirectionTimer) break;
                surferAction = 1 + floor(randomFloat() * 5);
                changeDirectionTimer = 40;
                break;
            case SLOWDOWN_OBJECT: if (!isInvincible()) {
                speed = 1;
                rushTimer = 0;
            }
            case RIPPLE_OBJECT: break;
            default: hitPlayer();
            }
            if (isNaughtySurferExists()) {
                float nx = naughtySurfer.x - offset + 32, ny = naughtySurfer.y - distance + 50;
                if (nx > cx - x && nx < cx + x && ny > cy - y && ny < cy + y) switch (type) {
                case SMALL_OBJECT: naughtySurfer.action = naughtySurfer.action == 0;
                case SLOWDOWN_OBJECT:
                case INTERACT_OBJECT:
                case EFFECT_OBJECT:
                case RIPPLE_OBJECT: break;
                default:
                    naughtySurfer.action = 2;
                    return;
                }
                if (!isInvincible() && !flyingTimer && playerX > nx - 32 && playerX < nx + 32 && playerY > ny - 50 && playerY < ny + 20) {
                    naughtySurfer.action = 2;
                    hitPlayer();
                }
            }
            if (enemyTimer > 100 && !enemyStoped) {
                float nx = enemyX - offset + 64, ny = enemyY - distance + 64;
                if (!isInvincible() && !flyingTimer && playerX > nx - 64 && playerX < nx + 64 && playerY > ny - 64 && playerY < ny + 64) {
                    kickDog();
                    paused = finished = enemyStoped = true;
                    animationTimer = 1;
                    heart = speed = 0;
                    surferAction = 7;
                    boardBrokenTimer = 1;
                    refreshScore();
                    return;
                }
                if (nx > cx - x && nx < cx + x && ny > cy - y && ny < cy + y) switch (type) {
                case INTERACT_OBJECT:
                case EFFECT_OBJECT:
                case RIPPLE_OBJECT: break;
                default: enemyStoped = true;
                }
            }
        }
    }
}

void drawNaughtySurfer(NVGcontext* ctx) {
    if (distance > 1000 && randomDouble() > 0.9995) {
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
    if (paused) return;
    distance += speed;
    float ratio = 0;
    switch (surferAction) {
    case 1:
        ratio = -0.6F;
        break;
    case 2:
        ratio = -0.2F;
        break;
    case 5:
        ratio = 0.6F;
        break;
    case 4: ratio = 0.2F;
    }
    offset += ratio * speed;
    if (rushTimer) {
        if (rushTimer >= 600) {
            rushTimer = 0;
            speed = initialSpeed;
        } else rushTimer++;
    }
    if (speed < initialSpeed) speed *= 1.01F;
}

void drawStarterViewer(NVGcontext* ctx) {
    float alpha = ((float)animationTimer) / ((float)ANIMATION_TIMER_MAX_VALUE);
    float center = width / 2.0F, oLeft = (width - 64) / 2.0F, left = oLeft - surfer * 84, top = height * SURFER_TOP;
    for (int i = 0; i < 8; i++, left += 84) drawSurferOrigin(ctx, i,
        surfer == i ? 1 + (boardTimer >= 60 ? 4 - floor((boardTimer - 60) / 12.0) : floor(boardTimer / 12.0)) : 3, alpha, left, top);
    nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, alpha * 200));
    nvgStrokeWidth(ctx, 2);

    nvgBeginPath(ctx);
    if (surfer > 0) {
        nvgMoveTo(ctx, oLeft - 6, top + 14);
        nvgLineTo(ctx, oLeft - 16, top + 30);
        nvgLineTo(ctx, oLeft - 6, top + 46);
    }

    if (surfer < 7) {
        nvgMoveTo(ctx, oLeft + 70, top + 14);
        nvgLineTo(ctx, oLeft + 80, top + 30);
        nvgLineTo(ctx, oLeft + 70, top + 46);
    }

    nvgRoundedRect(ctx, center - 100, top + 87, 20, 20, 3);
    nvgRoundedRect(ctx, center - 124, top + 87, 20, 20, 3);
    nvgRoundedRect(ctx, center - 140, top + 127.5, 100, 20, 3);
    nvgStroke(ctx);

    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    nvgFillColor(ctx, nvgRGBA(0, 0, 0, alpha * 230));
    

    nvgFontSize(ctx, 50);
    nvgText(ctx, center, top - 100, "LET'S SURF!", NULL);

    nvgFontSize(ctx, 20);
    nvgText(ctx, center, top + 90, "A D TO SWITCH SURFER", NULL);
    nvgText(ctx, center, top + 130, "SPACEBAR TO GO SURFING!", NULL);

    nvgFontSize(ctx, 22);
    char str[200];
    sprintf(str, "Random seed: %d", randomSeed);
    nvgText(ctx, center, height - 60, str, NULL);
    if (editMode) nvgFillColor(ctx, nvgRGB(233, 30, 99));
    sprintf(str, "Hello, %s!", username);
    nvgText(ctx, center, height - 90, str, NULL);
}

void drawFinishViewer(NVGcontext* ctx) {
    float centerX = width / 2.0F, centerY = height / 2.0F - 50, textTop = centerY + 30;
    float alpha = ((float) animationTimer) / ((float)ANIMATION_TIMER_MAX_VALUE);
    nvgBeginPath(ctx);
    nvgFillColor(ctx, nvgRGBA(255, 255, 255, alpha * 190));
    nvgRect(ctx, 0, 0, width, height);
    nvgFill(ctx);

    nvgFontSize(ctx, 50);
    nvgFillColor(ctx, nvgRGBA(0, 0, 0, alpha * 230));

    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
    nvgText(ctx, centerX, centerY - 72, "GAME OVER!", NULL);
    nvgFontSize(ctx, 22);
    nvgText(ctx, centerX, centerY, "Press SPACEBAR to start a new round.", NULL);

    int score = getScore();
    nvgFontSize(ctx, 18);
    nvgText(ctx, centerX, textTop += 10, "------- Scores -------", NULL);

    nvgFontSize(ctx, 16);
    char str[100];
    bool flag = true;
    for (int i = 0; i < 10; i++) {
        if (!scores[i].score) return;
        textTop += 20;
        if (flag && !isNotRecordScore() && scores[i].score == score && !strcmp(username, scores[i].username)) {
            nvgFillColor(ctx, nvgRGBA(233, 30, 99, alpha * 255));
            sprintf(str, i ? "%s: %d" : "%s: %d (NEW RECORD!)", username, score);
            flag = false;
        } else {
            nvgFillColor(ctx, nvgRGBA(0, 0, 0, alpha * 200));
            sprintf(str, "%s: %d", scores[i].username, scores[i].score);
        }
        nvgText(ctx, centerX, textTop, str, NULL);
    }
    if (flag) {
        nvgFillColor(ctx, nvgRGBA(233, 30, 99, alpha * 255));
        sprintf(str, isNotRecordScore() ? "%s %d (NOT RECORDED)" : "%s %d", username, score);
        nvgText(ctx, centerX, textTop + 20, str, NULL);
    }

    nvgStrokeColor(ctx, nvgRGBA(0, 0, 0, alpha * 200));
    nvgStrokeWidth(ctx, 2);
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, centerX - 164, centerY - 4, 115, 24, 3);
    nvgStroke(ctx);
}

void drawEnemy(NVGcontext* ctx) {
    if (boardBrokenTimer) if (boardBrokenTimer < 59) boardBrokenTimer++;
    if (++enemyTimer == 160) enemyTimer = 100;
    if (enemyTimer < 100) return;
    if (enemyY + 200 < distance) {
        enemyX = enemyY = enemyTimer = 0;
        enemyStoped = false;
        return;
    }
    if (enemyStoped) drawImage(ctx, enemyImage, 1, 128, boardBrokenTimer / 10 * 128, 128, 128, enemyX - offset, enemyY - distance);
    else {
        float tx = offset + playerX - 64, ty = distance + playerY - 64;
        if (ty - enemyY > 6) enemyY += 4.2F;
        enemyX += 2.0F * (enemyX > tx ? -1 : enemyX == tx ? 0 : 1);
        if (enemyX - tx < 2.5F) enemyX = tx;
    }
    drawImage(ctx, enemyImage, 1, 0, ((enemyTimer - 100) / 10) * 128, 128, 128, enemyX - offset, enemyY - distance + 70);
}

void drawCheatWarning(NVGcontext* ctx) {
    if (!cheatWarningTimer) return;
    if (cheatWarningTimer < ANIMATION_TIMER_MAX_VALUE) cheatWarningTimer++;
    float w = SETTING_WIDTH * 1.47F, left = -(1.0F - ((float)cheatWarningTimer) / ((float)ANIMATION_TIMER_MAX_VALUE)) * w - 3;
    nvgFillColor(ctx, nvgRGBA(0, 0, 0, 170));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, left, SETTING_TOP, w, 60, 2);
    nvgFill(ctx);

    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFillColor(ctx, nvgRGBA(255, 255, 255, 230));
    nvgFontSize(ctx, 15);
    nvgTextBox(ctx, left + 12, SETTING_TOP + 10, w - 10, "Cheat code activated! Your score will not be saved this round!", NULL);
}

void draw(NVGcontext* ctx) {
    if (animationTimer) if (animationTimer < ANIMATION_TIMER_MAX_VALUE) animationTimer++;
    if (++boardTimer == 120) { boardTimer = 0; }
    if (started) {
        if (changeDirectionTimer > 0) changeDirectionTimer--;
        if (!finished) {
            calcOffset();
            if (fallTimer && fallTimer++ > 200) {
                fallTimer = 0;
                surferAction = 0;
                paused = true;
                speed = 0;
                invincibleTimer = 1;
            }
            if (invincibleTimer && surferAction != 0) {
                if (invincibleTimer++ > 300) invincibleTimer = 0;
            }
            if (flyingTimer > 0) flyingTimer--;
        }
    }
    nvgBeginPath(ctx);
    float center = width / 2.0F;
    nvgFillPaint(ctx, nvgLinearGradient(ctx, center, 0, center, height, nvgRGB(56, 194, 238), nvgRGB(46, 195, 208)));
    nvgRect(ctx, 0, 0, width, height);
    nvgFill(ctx);
    drawImage(ctx, backgroundImage, 1, (int)offset % 256, (int)distance % 256, width, height, 0, 0); // Background

    if (started) {
        generateObjects();
        drawObjects(ctx);
        if (enemyTimer) drawEnemy(ctx);
        drawSurfer(ctx);
        drawNaughtySurfer(ctx);
        if (finished) drawFinishViewer(ctx);
        drawCheatWarning(ctx);
    } else drawStarterViewer(ctx);
    drawStatusBar(ctx);
}
