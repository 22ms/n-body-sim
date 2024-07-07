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

int N = 65'536;
float timeScale = 1.0f;

int main(int, char**)
{
    // Initialization, use of Singleton pattern to encapsulate global states
    GLWrapper& glWrapper = GLWrapper::GetInstance();
    glWrapper.Initialize(1280, 720, "N-body simulation", &N);

    CLWrapper& clWrapper = CLWrapper::GetInstance();
    clWrapper.Initialize(glWrapper.Window, glWrapper.GetPosGLBO(), glWrapper.GetVelocities(), &N, &timeScale);

    ImGuiWrapper& imGuiWrapper = ImGuiWrapper::GetInstance();
    imGuiWrapper.Initialize(glWrapper.Window, &N, &timeScale);

    // Render loop
    while (!glWrapper.ShouldClose()) {
        glWrapper.Render();
        imGuiWrapper.Render();
        clWrapper.SimulateTimestep();

        glWrapper.SwapBuffers();
    }

    return 0;
}