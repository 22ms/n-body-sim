#ifndef GL_WRAPPER_HPP
#define GL_WRAPPER_HPP

typedef struct _cl_command_queue* cl_command_queue;

class Shader;
class Camera;

struct GLFWwindow;
struct Position;
struct Velocity;

namespace glwrapper {

    void Initialize(int width, int height, const char* title, unsigned int* nPtr, void (*worldGeneratorPtr)(Position*&, Velocity*&, const int));
    void Render(cl_command_queue cmdQueue);
    void SwapBuffers();
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

#endif // GL_WRAPPER_HPP