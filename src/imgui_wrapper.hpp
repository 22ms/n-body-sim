#ifndef IMGUI_WRAPPER_HPP
#define IMGUI_WRAPPER_HPP

class GLFWwindow;
class Position;
class Velocity;

namespace imguiwrapper {
    void Initialize (GLFWwindow* glWindow, float* mainCameraSpeedPtr, unsigned int* nPtr, float* timeScalePtr, void (*worldGeneratorPtr)(Position*&, Velocity*&, const int));
    void Display();
}

#endif