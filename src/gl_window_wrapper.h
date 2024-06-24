#define GLEW_BUILD
#define GLFW_DLL
#define GL_SILENCE_DEPRECATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "shader.h"

class GLWindowWrapper {
public:
    GLWindowWrapper(int width, int height, char* title, const char* glsl_version, int* N);
    ~GLWindowWrapper();

    GLFWwindow* window = nullptr;
    void render();
private:
    void updateBodiesVertices();
    void processInput(GLFWwindow *window);

    int _previousN;
    unsigned int _VBO, _VAO;

    int* _N = nullptr;
    float* _vertices = nullptr;
    Shader* _shader = nullptr;
};