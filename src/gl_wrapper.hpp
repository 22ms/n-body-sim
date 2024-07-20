#pragma once

#include <vector>
#include <memory>

namespace camera {
    class Camera;
}

struct GLFWwindow;

namespace glwrapper {
    extern void Initialize();
    extern void Render();
    extern void SwapBuffers();
    extern void Cleanup();
    extern bool ShouldClose();

    extern GLFWwindow* Window;
    extern std::unique_ptr<camera::Camera> MainCamera;

    extern unsigned int ParticleBuffer;
    extern float DeltaTime;
}