#ifndef IMGUI_WRAPPER_HPP
#define IMGUI_WRAPPER_HPP

namespace worldgenerators {
    enum class GeneratorType;
}

class GLFWwindow;
class Position;
class Velocity;

namespace imguiwrapper {
    void Initialize (GLFWwindow* glWindow, float* mainCameraSpeedPtr, unsigned int* nPtr, float* timeScalePtr, worldgenerators::GeneratorType* generatorType);
    void Display();
}

#endif