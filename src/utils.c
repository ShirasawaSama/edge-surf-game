#include "utils.h"
#include <stdlib.h>
#include <string.h>
#if defined _WIN32
#include <Windows.h>
#elif defined linux
#include <unistd.h>
#include <pwd.h>
#endif

double randomDouble() {
	return (double) rand() / RAND_MAX;
}
float randomFloat() {
	return (float)rand() / RAND_MAX;
}

void drawImage(NVGcontext* vg, int image, float alpha, float sx, float sy, float w, float h, float x, float y) {
	int iw, ih;
	NVGpaint img;

	nvgImageSize(vg, image, &iw, &ih);

	img = nvgImagePattern(vg, x - sx, y - sy, iw, ih, 0, image, alpha);
	nvgBeginPath(vg);
	nvgRect(vg, x, y, w, h);
	nvgFillPaint(vg, img);
	nvgFill(vg);
}

void getUserName(char* ch) {
#if defined _WIN32
	DWORD len = 100;
	GetUserNameA(ch, &len);
#elif defined linux
	struct passwd* pwd = getpwuid(getid());
	strcpy(ch, pwd->pw_name);
#endif
}
