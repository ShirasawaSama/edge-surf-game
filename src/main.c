#pragma warning(disable:4244)
#define MINIAUDIO_IMPLEMENTATION
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "nanovg.h"
#include "nanovg_gl.h"
#include "main.h"
#include "game.h"
#include "miniaudio.h"

int width, height, fps;
bool playPaused = false;

void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	if (playPaused) return;
	ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
	if (pDecoder != NULL) ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL, MA_TRUE);
	(void)pInput;
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
	deviceConfig.dataCallback = dataCallback;
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
	return 0;
}

int main() {
	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}
	GLFWwindow* window = glfwCreateWindow(1000, 800, "Edge Surf Game", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
	#ifndef _WIN32
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#endif
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_MULTISAMPLE);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glGetError();
	NVGcontext* ctx = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (ctx == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}
	initGame(ctx, window);
	int res = playMusic();
	if (res != 0) return res;
	printf("Loaded!\n");
	double prevT = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		nvgBeginFrame(ctx, width, height, (float) width / (float) height);
		double t = glfwGetTime();
		fps = round(1.0 / (t - prevT));
		prevT = t;
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
