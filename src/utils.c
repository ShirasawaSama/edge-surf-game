#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <GLFW/glfw3.h>
#define URL(CMD) CMD " https://github.com/ShirasawaSama/edge-surf-game"
#if defined _WIN32
#include <Windows.h>
#define COMMAND URL("start")
#elif defined linux
#include <unistd.h>
#include <pwd.h>
#define COMMAND URL("xdg-open")
#else
#define COMMAND URL("open")
#endif

double randomDouble() {
    return ((double) rand()) / ((double) RAND_MAX);
}
float randomFloat() {
    return ((float)rand()) / ((float)RAND_MAX);
}

void drawImage(NVGcontext* vg, int image, float alpha, float sx, float sy, float w, float h, float x, float y) {
    int iw, ih;
    NVGpaint img;

    nvgImageSize(vg, image, &iw, &ih);

    img = nvgImagePattern(vg, x - sx, y - sy, (float)iw, (float)ih, 0, image, alpha);
    nvgBeginPath(vg);
    nvgRect(vg, x, y, w, h);
    nvgFillPaint(vg, img);
    nvgFill(vg);
}

void openUrl() { system(COMMAND); }

void getUserName(char* ch) {
#if defined _WIN32
    DWORD len = 100;
    GetUserNameA(ch, &len);
#elif defined linux
    struct passwd* pwd = getpwuid(getgid());
    strcpy(ch, pwd->pw_name);
#elif defined __APPLE__
    strcpy(ch, getenv("USER"));
#else
    * ch = 0;
#endif
    if (!*ch) strcpy(ch, "Unknown");
}

bool isCapsLockHold(int mods) {
    if (mods & GLFW_MOD_CAPS_LOCK) return true;
#if defined _WIN32
    return GetKeyState(VK_CAPITAL) & 1;
#else
    return false;
#endif
}
