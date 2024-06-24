class GLFWwindow;

class ImGuiWindowWrapper {
public:
    ImGuiWindowWrapper(GLFWwindow* window, const char* glsl_version, int* N);
    ~ImGuiWindowWrapper();

    void render();
private:
    void setStyleGruvbox();
    int* _N;
};