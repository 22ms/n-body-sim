#define GLEW_BUILD
#define GLFW_DLL
#define GL_SILENCE_DEPRECATION

class Shader;
class GLFWwindow;

class GLWindowWrapper {
public:
    GLWindowWrapper(int width, int height, const char* title, int* N);
    ~GLWindowWrapper();

    GLFWwindow* window = nullptr;
    bool shouldClose();
    void swapBuffers();
    void render();
    void modifyVertexBuffer(float* vertices);
private:
    void expandVertexBuffer();
    void processInput(GLFWwindow *window);

    int _previousN;
    unsigned int _VBO, _VAO;

    int* _N = nullptr;
    float* _vertices = nullptr;
    Shader* _shader = nullptr;
};