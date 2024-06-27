#ifndef GL_WRAPPER_H
#define GL_WRAPPER_H

#define GLEW_BUILD
#define GL_SILENCE_DEPRECATION

class Shader;

struct GLFWwindow;
struct xyzm;

class GLWrapper {
public:
    GLWrapper(int width, int height, const char* title, int* N);
    ~GLWrapper();

    GLFWwindow* window = nullptr;
    unsigned int* getPosGLBO();
    bool shouldClose();
    void swapBuffers();
    void render();
    void modifyVertexBuffer(float* vertices);
private:
    void expandVertexBuffer();
    void processInput(GLFWwindow *window);

    int _previousN;
    unsigned int _posGLBO, _posGLAO;

    int* _N = nullptr;
    xyzm* _positions = nullptr;
    Shader* _shader = nullptr;
};

#endif