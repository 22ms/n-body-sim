#include <GL/glew.h>
#include <CL/cl.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <exception>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "gl_wrapper.hpp"
#include "shader.hpp"
#include "utilities.hpp"

// Extern

GLFWwindow* glWindow = nullptr;
Camera* glMainCamera = nullptr;
xyzm* glPositions = nullptr;
vxvyvz* glVelocities = nullptr;

unsigned int glPosBuffer;
int glWidth;
int glHeight;
float glDeltaTime;
float* glMainCameraSpeedPtr = nullptr;

// Static

Shader* shader = nullptr;

static unsigned int posAttribute;

static unsigned int* nPtr = nullptr;
static unsigned int previousN;

static float lastX;
static float lastY;
static float lastFrameTime;

static bool firstMouse;
static bool captureMouse;

void fillVertexBuffers();
void processKeyInput();

void errorCallback(int error, const char* description);
void framebufferSizeCallback(GLFWwindow* glWindow, int width, int height);
void mouseCallback(GLFWwindow* glWindow, double xposIn, double yposIn);
void mouseButtonCallback(GLFWwindow* glWindow, int button, int action, int mods);
void scrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset);

void glInitialize(int initialWidth, int initialHeight, const char* title, unsigned int* _nPtr)
{
    glWidth = initialWidth;
    glHeight = initialHeight;
    nPtr = _nPtr;
    previousN = *nPtr;

    glfwSetErrorCallback(errorCallback);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glWindow = glfwCreateWindow(glWidth, glHeight, title, nullptr, nullptr);

    glfwMakeContextCurrent(glWindow);
    glfwSwapInterval(1); // Enable vsync
    glViewport(0, 0, glWidth, glHeight);
    glfwSetFramebufferSizeCallback(glWindow, framebufferSizeCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetMouseButtonCallback(glWindow, mouseButtonCallback);
    glfwSetScrollCallback(glWindow, scrollCallback);

    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(glWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLenum err = glewInit();
    fillVertexBuffers();

    glGenBuffers(1, &glPosBuffer);
    glGenVertexArrays(1, &posAttribute);

    glBindVertexArray(posAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, glPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*nPtr) * 4, glPositions, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    shader = new Shader("shaders/basic.vert", "shaders/basic.frag");
    shader->Use();

    glMainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    glMainCameraSpeedPtr = &(glMainCamera->MovementSpeed);
    lastX = glWidth / 2.0f;
    lastY = glHeight / 2.0f;
    firstMouse = true;
    captureMouse = false;
    glDeltaTime = 0.0f;
    lastFrameTime = 0.0f;

}

void glRender(cl_command_queue cmdQueue)
{
    float currentFrameTime = static_cast<float>(glfwGetTime());
    glDeltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    processKeyInput();

    glm::mat4 projection = glm::perspective(glm::radians(glMainCamera->Zoom), (float)glWidth / (float)glHeight, 0.1f, 100.0f);
    shader->SetMat4("projection", projection);

    glm::mat4 view = glMainCamera->GetViewMatrix();
    shader->SetMat4("view", view);

    if (*nPtr != previousN) {
        glFinish();
        clFinish(cmdQueue);
        fillVertexBuffers();
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*nPtr) * 4, glPositions, GL_DYNAMIC_DRAW);
        printf("Updated GL Buffer with n: %d\n", (*nPtr));
    }
    glDrawArrays(GL_POINTS, 0, *nPtr);

    previousN = *nPtr;

    GLenum err = glGetError();
    if (err != 0) {
        printf("OpenGL error: 0x%x\n", err);
        std::terminate();
    }
}

void glSwapBuffers()
{
    glfwSwapBuffers(glWindow);
}

bool glShouldClose() {
    return glfwWindowShouldClose(glWindow);
}

void fillVertexBuffers()
{
    if (glPositions != nullptr) {
        delete[] glPositions;
    }

    if (glVelocities != nullptr) {
        delete[] glVelocities;
    }

    glPositions = new xyzm[*nPtr];
    glVelocities = new vxvyvz[*nPtr];

    float endRadius = 1.0f;
    float spacing = endRadius / *nPtr;

    // set positions
    for (int i = 0; i < *nPtr; i++) {
        double theta = acos(2 * rand() / double(RAND_MAX) - 1); // Polar angle
        double phi = rand() / double(RAND_MAX) * 2 * M_PI; // Azimuthal angle
        double radius = endRadius; // Radius of the sphere

        glPositions[i].x = radius * sin(theta) * cos(phi);
        glPositions[i].y = radius * sin(theta) * sin(phi);
        glPositions[i].z = radius * cos(theta);
        glPositions[i].m = 1;
    }

    // set velocities
    for (int i = 0; i < *nPtr; i++) {
        glVelocities[i].vx = 0;
        glVelocities[i].vy = 0;
        glVelocities[i].vz = 0;
    }

    printf("Updated velocities with n: %d\n", (*nPtr));
}

void processKeyInput()
{
    if (glfwGetKey(glWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(glWindow, true);

    if (glfwGetKey(glWindow, GLFW_KEY_W) == GLFW_PRESS)
        glMainCamera->ProcessKeyboard(FORWARD, glDeltaTime);
    if (glfwGetKey(glWindow, GLFW_KEY_S) == GLFW_PRESS)
        glMainCamera->ProcessKeyboard(BACKWARD, glDeltaTime);
    if (glfwGetKey(glWindow, GLFW_KEY_A) == GLFW_PRESS)
        glMainCamera->ProcessKeyboard(LEFT, glDeltaTime);
    if (glfwGetKey(glWindow, GLFW_KEY_D) == GLFW_PRESS)
        glMainCamera->ProcessKeyboard(RIGHT, glDeltaTime);
}

void errorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void framebufferSizeCallback(GLFWwindow* glWindow, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* glWindow, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (captureMouse) {
        glMainCamera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void mouseButtonCallback(GLFWwindow* glWindow, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        captureMouse = true;
        glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        captureMouse = false;
        glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void scrollCallback(GLFWwindow* glWindow, double xoffset, double yoffset)
{
    glMainCamera->ProcessMouseScroll(yoffset);
}