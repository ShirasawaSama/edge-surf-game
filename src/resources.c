#include "resources.h"

int backgroundImage, surferImage, playerImage, boardImage, interfaceImage, naughtySurferImage,
	objectsSmallImage, objectsBigImage, slowdownImage, rippleImage, ambientImage, sandBarImage, islandImage, interactImage, effectsImage;
int objectTextures[10];

void loadResources(NVGcontext* ctx) {
	backgroundImage = nvgCreateImage(ctx, "../src/images/water256.png", NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
	surferImage = nvgCreateImage(ctx, "../src/images/surfer64.png", 0);
	playerImage = nvgCreateImage(ctx, "../src/images/player64.png", 0);
	boardImage = nvgCreateImage(ctx, "../src/images/surfboard64.png", 0);
	interfaceImage = nvgCreateImage(ctx, "../src/images/interface24.png", 0);
	interfaceImage = nvgCreateImage(ctx, "../src/images/interface24.png", 0);
	naughtySurferImage = nvgCreateImage(ctx, "../src/images/surfer64.png", 0);
	objectTextures[0] = objectsSmallImage = nvgCreateImage(ctx, "../src/images/objects32.png", 0);
	objectTextures[1] = objectsBigImage = nvgCreateImage(ctx, "../src/images/objects64.png", 0);
	objectTextures[2] = slowdownImage = nvgCreateImage(ctx, "../src/images/slowdown64.png", 0);
	objectTextures[3] = rippleImage = nvgCreateImage(ctx, "../src/images/ripple96.png", 0);
	objectTextures[4] = ambientImage = nvgCreateImage(ctx, "../src/images/ambient64.png", 0);
	objectTextures[5] = sandBarImage = nvgCreateImage(ctx, "../src/images/sandbar256.png", 0);
	objectTextures[6] = islandImage = nvgCreateImage(ctx, "../src/images/island1280.png", 0);
	objectTextures[7] = interactImage = nvgCreateImage(ctx, "../src/images/interact64.png", 0);
	objectTextures[8] = effectsImage = nvgCreateImage(ctx, "../src/images/effects128.png", 0);

	nvgCreateFont(ctx, "sans", "../src/FiraCode-Regular.ttf");
	nvgFontFace(ctx, "sans");
}

void freeResources(NVGcontext* ctx) {
	nvgDeleteImage(ctx, backgroundImage);
	nvgDeleteImage(ctx, surferImage);
	nvgDeleteImage(ctx, playerImage);
	nvgDeleteImage(ctx, boardImage);
	nvgDeleteImage(ctx, interfaceImage);
	nvgDeleteImage(ctx, naughtySurferImage);
	nvgDeleteImage(ctx, objectsSmallImage);
	nvgDeleteImage(ctx, objectsBigImage);
	nvgDeleteImage(ctx, slowdownImage);
	nvgDeleteImage(ctx, rippleImage);
	nvgDeleteImage(ctx, ambientImage);
	nvgDeleteImage(ctx, sandBarImage);
	nvgDeleteImage(ctx, islandImage);
	nvgDeleteImage(ctx, interactImage);
	nvgDeleteImage(ctx, effectsImage);
}
