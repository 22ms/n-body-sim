#ifndef CL_WRAPPER_HPP
#define CL_WRAPPER_HPP

typedef struct _cl_command_queue* cl_command_queue;

class GLFWwindow;
class Kernel;
struct Velocity;

void clInitialize(GLFWwindow* window, unsigned int* _glPosBufferPtr, Velocity* _velocities, unsigned int* _nPtr, float* _timeScale); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
void clSimulateTimestep();

extern cl_command_queue clCmdQueue;

#endif