// Author: Leander Kroth

// References:
// https://learnopengl.com/
// https://bashbaug.github.io/OpenCL-Docs/pdf/OpenCL_API.pdf
// https://web.engr.oregonstate.edu/~mjb/cs575/Handouts/opencl.opengl.vbo.1pp.pdf
// https://dournac.org/info/nbody_tutorial
#include <memory>

#include "gl_wrapper.hpp"
#include "cl_wrapper.hpp"
#include "imgui_wrapper.hpp"
#include "world_generators.hpp"

// Initial parameters, can be changed in UI
unsigned int N = 16'384; // Numbers of particles, has to be < MAX_N and power of two.
float timeScale = 1.0f; // Time scale of the simulation.
worldgens::SphereGenerator worldGenerator; // Particle configuration

int main(int, char**)
{
    worldgens::Initialize(worldGenerator);
    glwrapper::Initialize(1280, 720, "N-body simulation, O(n²)", &N, clwrapper::UpdateCLBuffers);
    clwrapper::Initialize(glwrapper::PosBuffer, &glwrapper::Velocities, N, &timeScale);
    imguiwrapper::Initialize(glwrapper::Window, glwrapper::MainCameraSpeedPtr, &N, &timeScale);

    // Render loop, order is important!
    while (!glwrapper::ShouldClose()) {
        glwrapper::Render();
        clwrapper::SimulateTimestep();
        imguiwrapper::Display();

        glwrapper::SwapBuffers();
    }
    

    glwrapper::Cleanup();
    worldgens::Cleanup();
}