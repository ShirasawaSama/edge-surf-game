#include "utils.h"
#include <stdlib.h>
#include <string.h>

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

	img = nvgImagePattern(vg, x - sx, y - sy, iw, ih, 0, image, alpha);
	nvgBeginPath(vg);
	nvgRect(vg, x, y, w, h);
	nvgFillPaint(vg, img);
	nvgFill(vg);
}

void openUrl() {
#if defined _WIN32
	char command[] = "start https://github.com/ShirasawaSama/edge-surf-game";
#elif defined linux
	char command[] = "xdg-open https://github.com/ShirasawaSama/edge-surf-game";
#else
	char command[] = "open https://github.com/ShirasawaSama/edge-surf-game";
#endif
	system(command);
}
