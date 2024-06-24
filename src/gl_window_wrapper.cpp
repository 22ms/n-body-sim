#include "gl_window_wrapper.h"

static void glfwErrorCallback (int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void framebufferSizeCallback (GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

GLWindowWrapper::GLWindowWrapper(int width, int height, char* title, const char* glsl_version, int* N) {
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
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);

    expandVertexBuffer();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 3, _vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    _shader = new Shader("shaders/basic.vert", "shaders/basic.frag");
}

GLWindowWrapper::~GLWindowWrapper() {
    glfwDestroyWindow(window);
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glfwTerminate();

    delete window;
    delete _N;
    delete[] _vertices;
    delete _shader;
}

void GLWindowWrapper::render () {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    processInput(window);

    if (*_N != _previousN) {
        expandVertexBuffer();
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (*_N) * 3, _vertices, GL_DYNAMIC_DRAW);
    }

    _shader->use();
    glBindVertexArray(_VAO);
    glDrawArrays(GL_POINTS, 0, *_N);

    _previousN = *_N;
}

void GLWindowWrapper::expandVertexBuffer() {
    int len = (*_N)*3;
    if (_vertices != nullptr) {
        delete[] _vertices;
    }
    _vertices = new float[len];
    for (int i = 0; i < len-2; i+=3) {
        _vertices[i] = i * 0.01;    // x
        _vertices[i+1] = i * 0.01;  // y
        _vertices[i+2] = 0;         // z
    }
}

void GLWindowWrapper::processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}