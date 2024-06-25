#define GLEW_BUILD
#define GLFW_DLL
#define GL_SILENCE_DEPRECATION

class Shader;
class GLFWwindow;
struct xyz;

class GLWindowWrapper {
public:
    GLWindowWrapper(int width, int height, const char* title, int* N);
    ~GLWindowWrapper();

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
    xyz* _positions = nullptr;
    Shader* _shader = nullptr;
};