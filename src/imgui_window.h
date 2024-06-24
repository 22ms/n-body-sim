#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class ImGuiWindow {
public:
    ImGuiWindow(GLFWwindow* window, const char* glsl_version, int* N);
    ~ImGuiWindow();

    void render();
private:
    void setStyleGruvbox();
    int* N;
};