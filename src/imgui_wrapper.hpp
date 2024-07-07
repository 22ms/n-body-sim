#ifndef IMGUI_WRAPPER_HPP
#define IMGUI_WRAPPER_HPP

class GLFWwindow;

class ImGuiWrapper {
    // Singleton pattern: https://stackoverflow.com/a/1008289/16951338
public:
    static ImGuiWrapper& GetInstance()
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
    void Initialize(GLFWwindow* window, int* N, float* cameraSpeed, float* timeScale); // Pointer to GLFWwindow not necessarily needed, but makes the dependency on an initialized OpenGL context obvious.
    void Display();

private:
    void setStyleGruvbox();
    int* m_N = nullptr;
    float* m_CameraSpeed = nullptr;
    float* m_TimeScale = nullptr;
};

#endif