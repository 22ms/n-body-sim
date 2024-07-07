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
    static GLWrapper& GetInstance()
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
    void Initialize(int width, int height, const char* title, int* N);

    GLFWwindow* Window = nullptr;
    unsigned int* GetPosGLBO();
    vxvyvz* GetVelocities();
    bool ShouldClose();
    void SwapBuffers();
    void Render();

    // Camera specific
    Camera* MainCamera = nullptr;
    float LastX;
    float LastY;
    bool FirstMouse;
    bool CaptureMouse;

private:
    void fillVertexBuffers();
    void processKeyInput(GLFWwindow* window);

    // Callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void errorCallback(int error, const char* description);
    static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    int m_Width, m_Height;
    float m_DeltaTime; // could be public, not needed yet
    float m_LastFrameTime;

    int m_PreviousN;
    unsigned int m_PosGLBO, m_PosGLAO;

    int* m_N = nullptr;
    xyzm* m_Positions = nullptr;
    vxvyvz* m_Velocities = nullptr;
    Shader* m_Shader = nullptr;
};

#endif