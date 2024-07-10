#ifndef IMGUI_WRAPPER_HPP
#define IMGUI_WRAPPER_HPP

class GLFWwindow;

void imGuiInitialize (GLFWwindow* glWindow, float* _mainCameraSpeedPtr, unsigned int* _nPtr, float* _timeScalePtr); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
void imGuiDisplay();

#endif