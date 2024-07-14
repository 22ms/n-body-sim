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

    void Initialize();
    void Render();
    void SwapBuffers();
    void Cleanup();
    bool ShouldClose();

    extern GLFWwindow* Window;
    extern std::unique_ptr<camera::Camera> MainCamera;
    extern std::vector<utilities::Position> Positions;
    extern std::vector<utilities::Velocity> Velocities;

    extern unsigned int PosBuffer;
    extern int CurrentWidth;
    extern int CurrentHeight;
    extern float DeltaTime;

} // namespace glwrapper