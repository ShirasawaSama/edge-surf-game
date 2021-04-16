#define MINIAUDIO_IMPLEMENTATION
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "nanovg.h"
#include "nanovg_gl.h"
#include "main.h"
#include "game.h"
#include "miniaudio.h"

int width, height;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
	if (pDecoder != NULL) ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL, MA_TRUE);
}

ma_decoder decoder;
ma_device device;
int playMusic() {
	ma_result result = ma_decoder_init_file("../src/bgm.mp3", NULL, &decoder);
	if (result != MA_SUCCESS) {
		printf("Failed to init.\n");
		return -1;
	}

	ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format = decoder.outputFormat;
	deviceConfig.playback.channels = decoder.outputChannels;
	deviceConfig.sampleRate = decoder.outputSampleRate;
	deviceConfig.dataCallback = data_callback;
	deviceConfig.pUserData = &decoder;

	if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
		printf("Failed to open playback device.\n");
		ma_decoder_uninit(&decoder);
		return -1;
	}

	if (ma_device_start(&device) != MA_SUCCESS) {
		printf("Failed to start playback device.\n");
		ma_device_uninit(&device);
		ma_decoder_uninit(&decoder);
		return -1;
	}
}

int main() {
	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(1000, 600, "Edge Surf Game", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
	glGetError();
	NVGcontext* ctx = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
	if (ctx == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}
	initGame(ctx, window);
	int res = playMusic();
	if (res != 0) return res;
	printf("Loaded!\n");
	while (!glfwWindowShouldClose(window)) {
		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(56.0F / 255, 194.0F / 255, 238.0F / 255, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		nvgBeginFrame(ctx, width, height, (float) width / (float) height);
		draw(ctx);
		nvgEndFrame(ctx);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	destoryGame(ctx);
	nvgDeleteGL3(ctx);
	glfwTerminate();
	ma_device_uninit(&device);
	ma_decoder_uninit(&decoder);
	return 0;
}
