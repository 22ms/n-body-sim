// Author: Leander Kroth

// References:
// https://iss.oden.utexas.edu/Publications/Papers/burtscher11.pdf
// https://learnopengl.com/
// https://bashbaug.github.io/OpenCL-Docs/pdf/OpenCL_API.pdf

#include "gl_window.h"
#include "imgui_window.h"

const char* GLSL_VERSION = "#version 130";
int N = 500;

int main(int, char**)
{
    // Initialization
    GLWindow glWindow(1280, 720, "N-body simulation", GLSL_VERSION, &N);
    ImGuiWindow imGuiWindow(glWindow.window, GLSL_VERSION, &N);

    // Render loop
    while (!glfwWindowShouldClose(glWindow.window)) {
        glWindow.render();
        imGuiWindow.render();

        glfwSwapBuffers(glWindow.window);
    }

    return 0;
}