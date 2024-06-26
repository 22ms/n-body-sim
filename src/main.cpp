// Author: Leander Kroth

// References:
// https://iss.oden.utexas.edu/Publications/Papers/burtscher11.pdf
// https://learnopengl.com/
// https://bashbaug.github.io/OpenCL-Docs/pdf/OpenCL_API.pdf
// https://web.engr.oregonstate.edu/~mjb/cs575/Handouts/opencl.opengl.vbo.1pp.pdf

#include "gl_wrapper.hpp"
#include "cl_wrapper.hpp"
#include "imgui_wrapper.hpp"

int N = 500;

int main(int, char**)
{
    // Initialization
    GLWrapper glWrapper(1280, 720, "N-body simulation", &N);
    CLWrapper clWrapper(glWrapper.window, &N, glWrapper.getPosGLBO());
    ImGuiWrapper imGuiWrapper(glWrapper.window, &N);

    // Render loop
    while (!glWrapper.shouldClose()) {
        glWrapper.render();
        imGuiWrapper.render();

        glWrapper.swapBuffers();
    }

    return 0;
}