// Author: Leander Kroth

// References:
// https://iss.oden.utexas.edu/Publications/Papers/burtscher11.pdf
// https://learnopengl.com/
// https://bashbaug.github.io/OpenCL-Docs/pdf/OpenCL_API.pdf

#include "gl_window_wrapper.h"
#include "imgui_window_wrapper.h"

const char* GLSL_VERSION = "#version 130";
int N = 500;

int main(int, char**)
{
    // Initialization
    GLWindowWrapper glWindowWrapper(1280, 720, "N-body simulation", GLSL_VERSION, &N);
    ImGuiWindowWrapper imGuiWindowWrapper(glWindowWrapper.window, GLSL_VERSION, &N);

    // Render loop
    while (!glfwWindowShouldClose(glWindowWrapper.window)) {
        glWindowWrapper.render();
        imGuiWindowWrapper.render();

        glfwSwapBuffers(glWindowWrapper.window);
    }

    return 0;
}