#include "gl_window_wrapper.h"
#include "shader.h"
#include "utilities.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

static void glfwErrorCallback (int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void framebufferSizeCallback (GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLWindowWrapper::GLWindowWrapper(int width, int height, const char* title, int* N) {
    _N = N;
    _previousN = *_N;
    
    glfwSetErrorCallback(glfwErrorCallback);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLenum err = glewInit();
    expandVertexBuffer();

    glGenBuffers(1, &_posGLBO);
    glGenVertexArrays(1, &_posGLAO);

    glBindVertexArray(_posGLAO);
    glBindBuffer(GL_ARRAY_BUFFER, _posGLBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 3, _positions, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    _shader = new Shader("shaders/basic.vert", "shaders/basic.frag");
    _shader->use();
}

unsigned int* GLWindowWrapper::getPosGLBO() {
    return &_posGLBO;
}

GLWindowWrapper::~GLWindowWrapper() {
    glfwDestroyWindow(window);
    glDeleteBuffers(1, &_posGLBO);
    glDeleteVertexArrays(1, &_posGLAO);
    glfwTerminate();

    delete window;
    delete _N;
    delete[] _positions;
    delete _shader;
}

bool GLWindowWrapper::shouldClose() {
    return glfwWindowShouldClose(window);
}

void GLWindowWrapper::render () {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    processInput(window);

    if (*_N != _previousN) {
        expandVertexBuffer();
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 3, _positions, GL_DYNAMIC_DRAW);
    }
    glDrawArrays(GL_POINTS, 0, *_N);

    _previousN = *_N;

    GLenum err = glGetError();
    if(err != 0) {
        printf("OpenGL error: 0x%x\n", err);
    }
}

void GLWindowWrapper::swapBuffers() {
    glfwSwapBuffers(window);
}

void GLWindowWrapper::expandVertexBuffer() {
    if (_positions != nullptr) {
        delete[] _positions;
    }
    _positions = new xyz[*_N];
    for (int i = 0; i < *_N; i++) {
        _positions[i].x = i * 0.01;
        _positions[i].y = i * 0.01;
        _positions[i].z = 0;
    }
}

void GLWindowWrapper::processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}