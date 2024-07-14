// Author: Leander Kroth

// References:
// https://learnopengl.com/
// https://bashbaug.github.io/OpenCL-Docs/pdf/OpenCL_API.pdf
// https://web.engr.oregonstate.edu/~mjb/cs575/Handouts/opencl.opengl.vbo.1pp.pdf
// https://dournac.org/info/nbody_tutorial
#include "world_state.hpp"
#include "gl_wrapper.hpp"
#include "cl_wrapper.hpp"
#include "imgui_wrapper.hpp"

int main(int, char**)
{
    worldstate::Initialize();
    glwrapper::Initialize();
    clwrapper::Initialize();
    imguiwrapper::Initialize();

    while (!glwrapper::ShouldClose()) {
        glwrapper::Render();
        clwrapper::SimulateTimestep();
        imguiwrapper::Display();

        glwrapper::SwapBuffers();
    }
    

    glwrapper::Cleanup();
    worldstate::Cleanup();
}