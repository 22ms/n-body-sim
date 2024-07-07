#ifndef IMGUI_WRAPPER_H
#define IMGUI_WRAPPER_H

class GLFWwindow;

class ImGuiWrapper {
    // Singleton pattern: https://stackoverflow.com/a/1008289/16951338
public:
    static ImGuiWrapper& getInstance()
    {
        static ImGuiWrapper instance;
        return instance;
    }

private:
    ImGuiWrapper() {};
    ImGuiWrapper(ImGuiWrapper const&);
    void operator=(ImGuiWrapper const&);

    // Member variables
public:
    void initialize(GLFWwindow* window, int* N, float* timeScale); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
    void render();

private:
    void setStyleGruvbox();
    int* _N = nullptr;
    float* _timeScale = nullptr;
};

#endif