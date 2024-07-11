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

unsigned int N = 1024; // Has to be power of 2
float timeScale = 1.0f; // Has to be >= 0

// DEBUG:
#include <stdio.h>
int numFramesAfterNChange = 1000;
bool startCounting = false;

int main(int, char**)
{
    glInitialize(1280, 720, "N-body simulation, O(n²)", &N);
    clInitialize(glWindow, &glPosBuffer, glVelocities, &N, &timeScale);
    imGuiInitialize(glWindow, glMainCameraSpeedPtr, &N, &timeScale);

    int i = 0;
    int frameCount = 0;
    int previousN = N;
    // Render loop, order is important!
    while (!glShouldClose() && i < numFramesAfterNChange) {
        printf("frame number: %d\n", frameCount);
        glRender(clCmdQueue);
        clSimulateTimestep();
        imGuiDisplay();

        glSwapBuffers();

        if (previousN != N) {
            previousN = N;
            startCounting = true;
        }

        if(startCounting) {
            i++;
        }
        frameCount++;
    }

    return 0;
}