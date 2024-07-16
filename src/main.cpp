// Author: Leander Kroth

// References:
// https://learnopengl.com/
// https://bashbaug.github.io/OpenCL-Docs/pdf/OpenCL_API.pdf
// https://web.engr.oregonstate.edu/~mjb/cs575/Handouts/opencl.opengl.vbo.1pp.pdf
// https://dournac.org/info/nbody_tutorial
// https://iss.oden.utexas.edu/Publications/Papers/burtscher11.pdf

#include "world_state.hpp"
#include "gl_wrapper.hpp"
#include "cl_wrapper.hpp"
#include "imgui_wrapper.hpp"

// Change initial parameters in config.cpp

int main(int, char**)
{
    worldstate::Initialize();
    glwrapper::Initialize();
    clwrapper::Initialize();
    imguiwrapper::Initialize();

    int i = 0;
    while (!glwrapper::ShouldClose()) {
        glwrapper::Render();
        clwrapper::SimulateTimestep();
        imguiwrapper::Display();

        glwrapper::SwapBuffers();
    }
    
    imguiwrapper::Cleanup();
    clwrapper::Cleanup();
    glwrapper::Cleanup();
    worldstate::Cleanup();
}