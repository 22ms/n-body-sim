// Author: Leander Kroth

// References:
// https://iss.oden.utexas.edu/Publications/Papers/burtscher11.pdf
// https://learnopengl.com/
// https://bashbaug.github.io/OpenCL-Docs/pdf/OpenCL_API.pdf
// https://web.engr.oregonstate.edu/~mjb/cs575/Handouts/opencl.opengl.vbo.1pp.pdf

#include "gl_window_wrapper.h"
#include "cl_wrapper.h"
#include "imgui_window_wrapper.h"

int N = 500;

int main(int, char**)
{
    // Initialization
    GLWindowWrapper glWindowWrapper(1280, 720, "N-body simulation", &N);
    CLWrapper clWrapper(glWindowWrapper.window, &N, glWindowWrapper.getPosGLBO());
    ImGuiWindowWrapper imGuiWindowWrapper(glWindowWrapper.window, &N);

    // Render loop
    while (!glWindowWrapper.shouldClose()) {
        glWindowWrapper.render();
        imGuiWindowWrapper.render();

        glWindowWrapper.swapBuffers();
    }

    return 0;
}