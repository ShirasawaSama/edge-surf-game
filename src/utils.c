#include "utils.h"
#include <stdlib.h>

double randomDouble() {
	return (double) rand() / RAND_MAX;
}
float randomFloat() {
	return (float)rand() / RAND_MAX;
}

void drawImage(NVGcontext* vg, int image, float alpha,
	float sx, float sy, float w, float h, // sprite location on texture
	float x, float y) {
	int iw, ih;
	NVGpaint img;

	nvgImageSize(vg, image, &iw, &ih);

	img = nvgImagePattern(vg, x - sx, y - sy, iw, ih, 0, image, alpha);
	nvgBeginPath(vg);
	nvgRect(vg, x, y, w, h);
	nvgFillPaint(vg, img);
	nvgFill(vg);
}
