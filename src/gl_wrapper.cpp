#include "gl_wrapper.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "utilities.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <exception>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void GLWrapper::initialize(int width, int height, const char* title, int* N)
{
    _width = width;
    _height = height;
    _N = N;
    _previousN = *_N;

    glfwSetErrorCallback(errorCallback);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    // glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLenum err = glewInit();
    fillVertexBuffers();

    glGenBuffers(1, &_posGLBO);
    glGenVertexArrays(1, &_posGLAO);

    glBindVertexArray(_posGLAO);
    glBindBuffer(GL_ARRAY_BUFFER, _posGLBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 4, _positions, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    _shader = new Shader("shaders/basic.vert", "shaders/basic.frag");
    _shader->use();

    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    lastX = _width / 2.0f;
    lastY = _height / 2.0f;
    firstMouse = true;
    captureMouse = false;
    _deltaTime = 0.0f;
    _lastFrameTime = 0.0f;
}

unsigned int* GLWrapper::getPosGLBO()
{
    return &_posGLBO;
}

vxvyvz* GLWrapper::getVelocities()
{
    return _velocities;
}

bool GLWrapper::shouldClose()
{
    return glfwWindowShouldClose(window);
}

void GLWrapper::render()
{
    float currentFrameTime = static_cast<float>(glfwGetTime());
    _deltaTime = currentFrameTime - _lastFrameTime;
    _lastFrameTime = currentFrameTime;

    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    processKeyInput(window);

    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)_width / (float)_height, 0.1f, 100.0f);
    _shader->setMat4("projection", projection);

    glm::mat4 view = camera->GetViewMatrix();
    _shader->setMat4("view", view);

    if (*_N != _previousN) {
        fillVertexBuffers();
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 4, _positions, GL_DYNAMIC_DRAW);
    }
    glDrawArrays(GL_POINTS, 0, *_N);

    _previousN = *_N;

    GLenum err = glGetError();
    if (err != 0) {
        printf("OpenGL error: 0x%x\n", err);
        std::terminate();
    }
}

void GLWrapper::swapBuffers()
{
    glfwSwapBuffers(window);
}

void GLWrapper::fillVertexBuffers()
{

    if (_positions != nullptr) {
        delete[] _positions;
    }

    if (_velocities != nullptr) {
        delete[] _velocities;
    }

    _positions = new xyzm[*_N];
    _velocities = new vxvyvz[*_N];

    float endRadius = 1.0f;
    float spacing = endRadius / *_N;

    // set positions
    for (int i = 0; i < *_N; i++) {
        double omega = rand() / double(RAND_MAX) * 2 * M_PI;
        double r = rand() / double(RAND_MAX) * endRadius;

        _positions[i].x = r * cos(omega);
        _positions[i].y = r * sin(omega);
        _positions[i].z = rand() / 100000.0f;
        _positions[i].m = 0.01;
    }

    // set velocities
    for (int i = 0; i < *_N; i++) {
        _velocities[i].vx = 0;
        _velocities[i].vy = 0;
        _velocities[i].vz = 0;
    }
}

void GLWrapper::errorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void GLWrapper::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void GLWrapper::mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    GLWrapper& instance = GLWrapper::getInstance();

    if (instance.firstMouse) {
        instance.lastX = xpos;
        instance.lastY = ypos;
        instance.firstMouse = false;
    }

    float xoffset = xpos - instance.lastX;
    float yoffset = instance.lastY - ypos; // reversed since y-coordinates go from bottom to top

    instance.lastX = xpos;
    instance.lastY = ypos;

    if (instance.captureMouse) {
        instance.camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void GLWrapper::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    GLWrapper& instance = GLWrapper::getInstance();
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        instance.captureMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        instance.captureMouse = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void GLWrapper::processKeyInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, _deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, _deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, _deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, _deltaTime);
}