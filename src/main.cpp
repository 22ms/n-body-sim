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

int N = 128;
float timeScale = 1.0f;

int main(int, char**)
{
    glInitialize(1280, 720, "N-body simulation", &N);
    clInitialize(glWindow, &glPosBuffer, glVelocities, &N, &timeScale);

    // ImGuiWrapper& imGuiWrapper = ImGuiWrapper::GetInstance();
    // imGuiWrapper.Initialize(window, &N, mainCameraSpeed, &timeScale);

    // Render loop, order is important!
    while (!glShouldClose()) {
        glRender();
        clSimulateTimestep();
        // imGuiWrapper.Display();

        glSwapBuffers();
    }

    return 0;
}