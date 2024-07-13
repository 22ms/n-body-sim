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

unsigned int N = 16'384; // Has to be power of 2, must be lower than MAX_N
float timeScale = 1.0f; // Has to be >= 0
worldgenerators::WorldGenerator* worldGenerator = new worldgenerators::TwoSpheresGenerator(); // Sets initial particle config

int main(int, char**)
{
    worldgenerators::Initialize(worldGenerator);
    glwrapper::Initialize(1280, 720, "N-body simulation, O(n²)", &N, clwrapper::UpdateCLBuffers, &worldGenerator);
    clwrapper::Initialize(glwrapper::PosBuffer, &glwrapper::Velocities, N, &timeScale);
    imguiwrapper::Initialize(glwrapper::Window, glwrapper::MainCameraSpeedPtr, &N, &timeScale, &worldGenerator);

    // Render loop, order is important!
    while (!glwrapper::ShouldClose()) {
        glwrapper::Render();
        clwrapper::SimulateTimestep();
        imguiwrapper::Display();

        glwrapper::SwapBuffers();
    }

    return 0;
}