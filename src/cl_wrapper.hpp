#ifndef CL_WRAPPER_HPP
#define CL_WRAPPER_HPP

class GLFWwindow;
class Kernel;
struct vxvyvz;

void clInitialize(GLFWwindow* window, unsigned int* _glPosBufferPtr, vxvyvz* _velocities, int* _nPtr, float* _timeScale); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
void clSimulateTimestep();

#endif