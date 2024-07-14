#pragma once

typedef struct _cl_command_queue* cl_command_queue;

namespace worldgens {
    class WorldGenerator;
}

class Shader;
class Camera;

struct GLFWwindow;
struct Position;
struct Velocity;

namespace glwrapper {

    void Initialize(int width, int height, const char* title, unsigned int* nPtr, void (*bufferUpdateCallback)(int));
    void Render();
    void SwapBuffers();
    void Cleanup();
    bool ShouldClose();

    extern GLFWwindow* Window;
    extern Camera* MainCamera;
    extern Position* Positions;
    extern Velocity* Velocities;

    extern unsigned int PosBuffer;
    extern int Width;
    extern int Height;
    extern float DeltaTime;
    extern float* MainCameraSpeedPtr;

} // namespace glwrapper