class GLFWwindow;

class ImGuiWindowWrapper {
public:
    ImGuiWindowWrapper(GLFWwindow* window, int* N); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
    ~ImGuiWindowWrapper();

    void render();
private:
    void setStyleGruvbox();
    int* _N;
};