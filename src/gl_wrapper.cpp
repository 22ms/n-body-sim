#include "gl_wrapper.hpp"
#include "shader.hpp"
#include "utilities.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <exception>
#include <cmath>

static void glfwErrorCallback (int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void framebufferSizeCallback (GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLWrapper::GLWrapper(int width, int height, const char* title, int* N) {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 4, _positions, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    _shader = new Shader("shaders/basic.vert", "shaders/basic.frag");
    _shader->use();
}

unsigned int* GLWrapper::getPosGLBO() {
    return &_posGLBO;
}

GLWrapper::~GLWrapper() {
    glfwDestroyWindow(window);
    window = nullptr;

    glDeleteBuffers(1, &_posGLBO);
    glDeleteVertexArrays(1, &_posGLAO);
    glfwTerminate();

    delete _N;
    delete[] _positions;
    delete _shader;
}

bool GLWrapper::shouldClose() {
    return glfwWindowShouldClose(window);
}

void GLWrapper::render () {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    processInput(window);

    if (*_N != _previousN) {
        expandVertexBuffer();
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 4, _positions, GL_DYNAMIC_DRAW);
    }
    glDrawArrays(GL_POINTS, 0, *_N);

    _previousN = *_N;

    GLenum err = glGetError();
    if(err != 0) {
        printf("OpenGL error: 0x%x\n", err);
        std::terminate();
    }
}

void GLWrapper::swapBuffers() {
    glfwSwapBuffers(window);
}

void GLWrapper::expandVertexBuffer() {
    if (_positions != nullptr) {
        delete[] _positions;
    }
    _positions = new xyzm[*_N];
    float spacing = 1;
    int length = (int) sqrt(*_N);
    float centerX = 0.5 * length * spacing * 0.01;
    float centerY = 0.5 * length * spacing * 0.01;
    for (int i = 0; i < *_N; i++) {
        if(i == 0) {
            _positions[i].x =  spacing * 0.01 * (i % length) - centerX;
            _positions[i].y =  spacing * 0.01 * (i / length)- centerY;
            _positions[i].z =  0;
            _positions[i].m =  0.02;
            continue;
        }
        _positions[i].x =  spacing * 0.01 * (i % length) - centerX;
        _positions[i].y =  spacing * 0.01 * (i / length)- centerY;
        _positions[i].z =  0;
        _positions[i].m =  0.01;
    }
}

void GLWrapper::processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}