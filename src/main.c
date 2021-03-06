#include <stdio.h>
#ifdef NANOVG_GLEW
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "nanovg.h"
#include "nanovg_gl.h"
#include "main.h"
#include "game.h"
#include "miniaudio.h"
#include "resources.h"

int width, height, fps;
bool playPaused = false, vSync = true;

void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    if (playPaused) return;
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder != NULL) ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL, MA_TRUE);
    (void)pInput;
}

ma_decoder decoder;
ma_device device;
void playMusic() {
    ma_result result = ma_decoder_init_file(BGM_PATH, NULL, &decoder);
    if (result != MA_SUCCESS) {
        printf("Failed to init music.\n");
        return;
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
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
    }
}

void onError(int code, const char* message) { printf("Code: %d, %s\n", code, message); }

int main() {
    glfwSetErrorCallback(onError);
    if (!glfwInit()) {
        printf("Failed to init GLFW.");
        return -1;
    }
#ifndef _WIN32
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(1200, 800, "Edge Surf Game", NULL, NULL);
    if (!window) {
        printf("Cannot create window.");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
#ifdef NANOVG_GLEW
    if (glewInit() != GLEW_OK) {
        printf("Could not init glew.\n");
        return -1;
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glGetError();
#endif
    NVGcontext* ctx = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    if (ctx == NULL) {
        printf("Could not init nanovg.\n");
        return -1;
    }
    glfwSwapInterval(vSync);
    initGame(ctx, window);
    playMusic();
    printf("Loaded!\n");
    glfwSetTime(0);
    double prevT = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        double t = glfwGetTime();
        fps = (int)round(1.0 / (t - prevT));
        if (vSync || t - prevT >= 0.01666666) {
            int fbWidth, fbHeight;
            glfwGetWindowSize(window, &width, &height);
            glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
            glViewport(0, 0, fbWidth, fbHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            nvgBeginFrame(ctx, (float)width, (float)height, (float)fbWidth / (float)width);
            draw(ctx);
            nvgEndFrame(ctx);
            glfwSwapBuffers(window);
            prevT = t;
        }
    }
    destoryGame(ctx);
    nvgDeleteGL3(ctx);
    glfwTerminate();
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    return 0;
}
