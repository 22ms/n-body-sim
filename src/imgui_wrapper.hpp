#ifndef IMGUI_WRAPPER_H
#define IMGUI_WRAPPER_H

class GLFWwindow;

class ImGuiWrapper {
public:
    ImGuiWrapper(GLFWwindow* window, int* N, float* timeScale); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
    ~ImGuiWrapper();

    void render();
private:
    void setStyleGruvbox();
    int* _N = nullptr;
    float* _timeScale = nullptr;
};

#endif