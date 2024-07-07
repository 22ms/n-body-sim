#ifndef GL_WRAPPER_H
#define GL_WRAPPER_H

#define GLEW_BUILD
#define GL_SILENCE_DEPRECATION

class Shader;

struct GLFWwindow;
struct xyzm;
struct vxvyvz;

class GLWrapper {

    // Singleton pattern: https://stackoverflow.com/a/1008289/16951338
public:
    static GLWrapper& getInstance()
    {
        static GLWrapper instance;
        return instance;
    }

private:
    GLWrapper() {};
    GLWrapper(GLWrapper const&);
    void operator=(GLWrapper const&);

    // Member variables
public:
    void initialize(int width, int height, const char* title, int* N);

    GLFWwindow* window = nullptr;
    unsigned int* getPosGLBO();
    vxvyvz* getVelocities();
    bool shouldClose();
    void swapBuffers();
    void render();

private:
    void fillVertexBuffers();
    void processInput(GLFWwindow* window);

    int _previousN;
    unsigned int _posGLBO, _posGLAO;

    int* _N = nullptr;
    xyzm* _positions = nullptr;
    vxvyvz* _velocities = nullptr;
    Shader* _shader = nullptr;
};

#endif