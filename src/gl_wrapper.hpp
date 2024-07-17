#pragma once

#include <vector>
#include <memory>

typedef struct _cl_command_queue* cl_command_queue;

namespace utilities {
    struct Position;
    struct Velocity;
}

namespace camera {
    class Camera;
}

namespace worldgens {
    class WorldGenerator;
}

class Shader;
class Camera;

struct GLFWwindow;

namespace glwrapper {
    extern void Initialize();
    extern void Render();
    extern void SwapBuffers();
    extern void Cleanup();
    extern bool ShouldClose();

    extern GLFWwindow* Window;
    extern std::unique_ptr<camera::Camera> MainCamera;
    extern float* ParticleArray;

    extern unsigned int ParticleBuffer;
    extern float DeltaTime;
}