// Author: Leander Kroth

// References:
// https://iss.oden.utexas.edu/Publications/Papers/burtscher11.pdf
// https://learnopengl.com/
// https://bashbaug.github.io/OpenCL-Docs/pdf/OpenCL_API.pdf
// https://web.engr.oregonstate.edu/~mjb/cs575/Handouts/opencl.opengl.vbo.1pp.pdf
// https://dournac.org/info/nbody_tutorial

#include "gl_wrapper.hpp"
#include "cl_wrapper.hpp"
#include "imgui_wrapper.hpp"
#include "world_generators.hpp"

unsigned int N = 1024; // Has to be power of 2, must be lower than MAX_N
float timeScale = 1.0f; // Has to be >= 0
void (*worldGeneratorPtr)(Position*&, Velocity*&, const int) = worldgenerators::SphereShellGenerator; // Specifies the initial particle configuration

int main(int, char**)
{
    glwrapper::Initialize(1280, 720, "N-body simulation, O(n²)", &N, worldGeneratorPtr);
    clwrapper::Initialize(&glwrapper::PosBuffer, glwrapper::Velocities, &N, &timeScale);
    imguiwrapper::Initialize(glwrapper::Window, glwrapper::MainCameraSpeedPtr, &N, &timeScale, worldGeneratorPtr);

    // Render loop, order is important!
    while (!glwrapper::ShouldClose()) {
        glwrapper::Render(clwrapper::cmdQueue);
        clwrapper::SimulateTimestep();
        imguiwrapper::Display();

        glwrapper::SwapBuffers();
    }

    return 0;
}