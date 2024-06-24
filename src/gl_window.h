#define GLEW_BUILD
#define GLFW_DLL
#define GL_SILENCE_DEPRECATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "shader.h"

class GLWindow {
public:
    GLWindow(int width, int height, char* title, const char* glsl_version, int* N);
    ~GLWindow();

    GLFWwindow* window = nullptr;
    void render();
private:
    void updateBodiesVertices();
    void processInput(GLFWwindow *window);

    int* _N;
    int _previousN;
    float* _vertices;
    unsigned int _VBO, _VAO;
    Shader _shader;
};