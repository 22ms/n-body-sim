#pragma once

typedef struct _cl_command_queue* cl_command_queue;

class GLFWwindow;
class Kernel;
struct Velocity;

namespace clwrapper {
    extern void Initialize(); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
    extern void UpdateInteropBuffer();
    extern void SimulateTimestep();
    extern void Cleanup();
}