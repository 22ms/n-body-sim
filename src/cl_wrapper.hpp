#ifndef CL_WRAPPER_HPP
#define CL_WRAPPER_HPP

typedef struct _cl_command_queue* cl_command_queue;

class GLFWwindow;
class Kernel;
struct Velocity;

namespace clwrapper {
    void Initialize(unsigned int glPosBuffer, Velocity** velocities, unsigned int n, float* timeScale); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
    void UpdateCLBuffers(int n);
    void SimulateTimestep();

    extern cl_command_queue cmdQueue;
}

#endif