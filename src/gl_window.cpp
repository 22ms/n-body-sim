#include "gl_window.h"

GLWindow::GLWindow(int width, int height, char* title, const char* glsl_version, int* N) {
    GLWindow::_N = N;
    GLWindow::_previousN = *_N;
    
    glfwSetErrorCallback(glfwErrorCallback);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLWindow::window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    GLenum err = glewInit();

    glfwMakeContextCurrent(GLWindow::window);
    glfwSwapInterval(1); // Enable vsync
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);

    GLWindow::updateBodiesVertices();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 3, _vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    _shader = Shader("shaders/basic.vert", "shaders/basic.frag");
}

GLWindow::~GLWindow() {
    glfwDestroyWindow(GLWindow::window);
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glfwTerminate();
}

void GLWindow::render () {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    processInput(GLWindow::window);

    if (*_N != _previousN) {
        GLWindow::updateBodiesVertices();
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 3, _vertices, GL_DYNAMIC_DRAW);
    }

    _shader.use();
    glBindVertexArray(_VAO);
    glDrawArrays(GL_POINTS, 0, 3);

    _previousN = *_N;
}

void GLWindow::updateBodiesVertices() {
    int len = (*_N)*3;
    _vertices = new float[len];
    for (int i = 0; i < len; i++) {
        _vertices[i] = i * 0.01; // for testing
    }
}

void GLWindow::processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static void glfwErrorCallback (int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void framebufferSizeCallback (GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}