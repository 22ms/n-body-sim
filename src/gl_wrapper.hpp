#ifndef GL_WRAPPER_H
#define GL_WRAPPER_H

#define GLEW_BUILD
#define GL_SILENCE_DEPRECATION

class Shader;
class Camera;

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

    // Camera specific
    Camera* camera = nullptr;
    float lastX;
    float lastY;
    bool firstMouse;
    bool captureMouse;

private:
    void fillVertexBuffers();
    void processKeyInput(GLFWwindow* window);

    // Callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void errorCallback(int error, const char* description);
    static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    int _width, _height;
    float _deltaTime; // could be public, not needed yet
    float _lastFrameTime;

    int _previousN;
    unsigned int _posGLBO, _posGLAO;

    int* _N = nullptr;
    xyzm* _positions = nullptr;
    vxvyvz* _velocities = nullptr;
    Shader* _shader = nullptr;
};

#endif