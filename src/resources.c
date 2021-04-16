#include "resources.h"

int backgroundImage, surferImage, playerImage, boardImage, objectsSmallImage;

void loadResources(NVGcontext* ctx) {
	backgroundImage = nvgCreateImage(ctx, "../src/images/water256.png", NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
	surferImage = nvgCreateImage(ctx, "../src/images/surfer64.png", 0);
	playerImage = nvgCreateImage(ctx, "../src/images/player64.png", 0);
	boardImage = nvgCreateImage(ctx, "../src/images/surfboard64.png", 0);
	objectsSmallImage = nvgCreateImage(ctx, "../src/images/objects32.png", 0);
}

void freeResources(NVGcontext* ctx) {
	nvgDeleteImage(ctx, backgroundImage);
	nvgDeleteImage(ctx, surferImage);
	nvgDeleteImage(ctx, playerImage);
	nvgDeleteImage(ctx, boardImage);
}
