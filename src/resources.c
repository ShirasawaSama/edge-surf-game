#include "resources.h"

int backgroundImage, surferImage, playerImage, boardImage, interfaceImage, naughtySurferImage, enemyImage,
	objectsSmallImage, objectsBigImage, slowdownImage, rippleImage, ambientImage, sandBarImage, islandImage, interactImage, effectsImage, docksImage;
int objectTextures[10];

void loadResources(NVGcontext* ctx) {
	backgroundImage = nvgCreateImage(ctx, ASSETS_PATH("images/water256.png"), NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
	surferImage = nvgCreateImage(ctx, ASSETS_PATH("images/surfer64.png"), 0);
	playerImage = nvgCreateImage(ctx, ASSETS_PATH("images/player64.png"), 0);
	boardImage = nvgCreateImage(ctx, ASSETS_PATH("images/surfboard64.png"), 0);
	interfaceImage = nvgCreateImage(ctx, ASSETS_PATH("images/interface24.png"), 0);
	naughtySurferImage = nvgCreateImage(ctx, ASSETS_PATH("images/surfer64.png"), 0);
	enemyImage = nvgCreateImage(ctx, ASSETS_PATH("images/enemy128.png"), 0);
	objectTextures[0] = objectsSmallImage = nvgCreateImage(ctx, ASSETS_PATH("images/objects32.png"), 0);
	objectTextures[1] = objectsBigImage = nvgCreateImage(ctx, ASSETS_PATH("images/objects64.png"), 0);
	objectTextures[2] = slowdownImage = nvgCreateImage(ctx, ASSETS_PATH("images/slowdown64.png"), 0);
	objectTextures[3] = rippleImage = nvgCreateImage(ctx, ASSETS_PATH("images/ripple96.png"), 0);
	objectTextures[4] = ambientImage = nvgCreateImage(ctx, ASSETS_PATH("images/ambient64.png"), 0);
	objectTextures[5] = sandBarImage = nvgCreateImage(ctx, ASSETS_PATH("images/sandbar256.png"), 0);
	objectTextures[6] = islandImage = nvgCreateImage(ctx, ASSETS_PATH("images/island1280.png"), 0);
	objectTextures[7] = interactImage = nvgCreateImage(ctx, ASSETS_PATH("images/interact64.png"), 0);
	objectTextures[8] = effectsImage = nvgCreateImage(ctx, ASSETS_PATH("images/effects128.png"), 0);
	objectTextures[9] = docksImage = nvgCreateImage(ctx, ASSETS_PATH("images/docks64.png"), 0);

	nvgCreateFont(ctx, "sans", ASSETS_PATH("FiraCode-Regular.ttf"));
	nvgFontFace(ctx, "sans");
}

void freeResources(NVGcontext* ctx) {
	nvgDeleteImage(ctx, backgroundImage);
	nvgDeleteImage(ctx, surferImage);
	nvgDeleteImage(ctx, playerImage);
	nvgDeleteImage(ctx, boardImage);
	nvgDeleteImage(ctx, interfaceImage);
	nvgDeleteImage(ctx, naughtySurferImage);
	nvgDeleteImage(ctx, enemyImage);
	nvgDeleteImage(ctx, objectsSmallImage);
	nvgDeleteImage(ctx, objectsBigImage);
	nvgDeleteImage(ctx, slowdownImage);
	nvgDeleteImage(ctx, rippleImage);
	nvgDeleteImage(ctx, ambientImage);
	nvgDeleteImage(ctx, sandBarImage);
	nvgDeleteImage(ctx, islandImage);
	nvgDeleteImage(ctx, interactImage);
	nvgDeleteImage(ctx, effectsImage);
	nvgDeleteImage(ctx, docksImage);
}
