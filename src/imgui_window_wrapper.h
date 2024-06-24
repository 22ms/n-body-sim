#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class ImGuiWindowWrapper {
public:
    ImGuiWindowWrapper(GLFWwindow* window, const char* glsl_version, int* N);
    ~ImGuiWindowWrapper();

    void render();
private:
    void setStyleGruvbox();
    int* _N;
};